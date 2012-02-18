// Andrew Naplavkov

#ifndef BRIG_DATABASE_CONNECTION_HPP
#define BRIG_DATABASE_CONNECTION_HPP

#include <brig/boost/geometry.hpp>
#include <brig/database/column.hpp>
#include <brig/database/column_detail.hpp>
#include <brig/database/command.hpp>
#include <brig/database/detail/deleter.hpp>
#include <brig/database/detail/link.hpp>
#include <brig/database/detail/linker.hpp>
#include <brig/database/detail/pool.hpp>
#include <brig/database/detail/sql_columns.hpp>
#include <brig/database/detail/sql_indexed_columns.hpp>
#include <brig/database/detail/sql_mbr.hpp>
#include <brig/database/detail/sql_srid.hpp>
#include <brig/database/detail/sql_tables.hpp>
#include <brig/database/detail/sql_vector_layer.hpp>
#include <brig/database/detail/sql_vector_layers.hpp>
#include <brig/database/detail/sqlite_table_detail.hpp>
#include <brig/database/global.hpp>
#include <brig/database/index_detail.hpp>
#include <brig/database/numeric_cast.hpp>
#include <brig/database/object.hpp>
#include <brig/database/rowset.hpp>
#include <brig/database/table_detail.hpp>
#include <brig/database/variant.hpp>
#include <brig/detail/string_cast.hpp>
#include <memory>
#include <vector>

namespace brig { namespace database {

template <bool Threading>
class connection {
  typedef detail::pool<Threading> pool_type;
  std::shared_ptr<pool_type> m_pl;
  std::shared_ptr<detail::link> get_link()  { return std::shared_ptr<detail::link>(m_pl->pull(), detail::deleter<pool_type>(m_pl)); }
public:
  explicit connection(std::shared_ptr<detail::linker> lkr) : m_pl(new pool_type(lkr))  {}
  std::shared_ptr<command> get_command()  { return get_link(); }
  std::vector<object> get_tables();
  table_detail<column_detail> get_table_detail(const object& tbl);
  std::vector<column> get_vector_layers();
  boost::box get_mbr(const object& tbl, const column_detail& col);
  std::shared_ptr<rowset> get_vector_layer
    ( const table_detail<column_detail>& tbl, const std::string& lr, const boost::box& box
    , const std::vector<std::string> cols = std::vector<std::string>(), int rows = -1
    );
}; // connection

template <bool Threading>
std::vector<object> connection<Threading>::get_tables()
{
  using namespace brig::database::detail;
  using namespace brig::detail;

  std::vector<object> tables;
  auto lnk = get_link();
  lnk->exec(sql_tables(lnk->system()));
  std::vector<variant> row;
  while (lnk->fetch(row))
  {
    object tbl;
    tbl.schema = string_cast<char>(row[0]);
    tbl.name = string_cast<char>(row[1]);
    tables.push_back(tbl);
  }
  return std::move(tables);
}

template <bool Threading>
table_detail<column_detail> connection<Threading>::get_table_detail(const object& tbl)
{
  using namespace brig::database::detail;
  using namespace brig::detail;

  auto lnk = get_link();
  const DBMS sys(lnk->system());
  if (SQLite == sys) return sqlite_table_detail(lnk, tbl);

  // columns
  table_detail<column_detail> meta;
  meta.table = tbl;
  lnk->exec(sql_columns(sys, tbl));
  std::vector<variant> row;
  while (lnk->fetch(row))
  {
    column_detail col;
    col.name = string_cast<char>(row[0]);
    col.type.schema = string_cast<char>(row[1]);
    col.type.name = string_cast<char>(row[2]);
    numeric_cast(row[3], col.chars);
    numeric_cast(row[4], col.precision);
    numeric_cast(row[5], col.scale);
    meta.columns.push_back(std::move(col));
  }

  // indexes
  lnk->exec(sql_indexed_columns(sys, tbl));
  index_detail idx;
  while (lnk->fetch(row))
  {
    object index;
    index.schema = string_cast<char>(row[0]);
    index.name = string_cast<char>(row[1]);

    if (!(index == idx.index))
    {
      if (VoidIndex != idx.type) meta.indexes.push_back(std::move(idx));

      idx = index_detail();
      idx.index = index;
      int primary(0), unique(0), spatial(0);
      numeric_cast(row[2], primary);
      numeric_cast(row[3], unique);
      numeric_cast(row[4], spatial);
      if (primary) idx.type = Primary;
      else if (unique) idx.type = Unique;
      else if (spatial) idx.type = Spatial;
      else idx.type = Duplicate;
    }

    const std::string col_name(string_cast<char>(row[5]));
    idx.columns.push_back(col_name);
    if (std::find_if(meta.columns.begin(), meta.columns.end(), [&](const column_detail& col){ return col.name == col_name; }) == meta.columns.end()) idx.type = VoidIndex; // expression

    int desc(0);
    if (numeric_cast(row[6], desc) && desc) idx.type = VoidIndex; // descending
  }
  if (VoidIndex != idx.type) meta.indexes.push_back(std::move(idx));

  // srid, epsg, type detail
  for (size_t i(0); i < meta.columns.size(); ++i)
  {
    const std::string sql(sql_srid(sys, tbl, meta.columns[i]));
    if (!sql.empty())
    {
      lnk->exec(sql);
      if (lnk->fetch(row))
      {
        numeric_cast(row[0], meta.columns[i].srid);
        if (row.size() > 1) numeric_cast(row[1], meta.columns[i].epsg);
        else meta.columns[i].epsg = meta.columns[i].srid;
        if (row.size() > 2) meta.columns[i].type_detail = string_cast<char>(row[2]);
      }
    }
  }
  return std::move(meta);
}

template <bool Threading>
std::vector<column> connection<Threading>::get_vector_layers()
{
  using namespace brig::database::detail;
  using namespace brig::detail;

  auto lnk = get_link();
  const DBMS sys(lnk->system());
  std::vector<variant> row;
  if (SQLite == sys)
  {
    lnk->exec(sql_tables(sys, "GEOMETRY_COLUMNS"));
    if (!lnk->fetch(row)) return std::vector<column>();
  }

  std::vector<column> drawings;
  lnk->exec(sql_vector_layers(sys));
  while (lnk->fetch(row))
  {
    column drw;
    drw.table.schema = string_cast<char>(row[0]);
    drw.table.name = string_cast<char>(row[1]);
    drw.name = string_cast<char>(row[2]);
    drawings.push_back(drw);
  }
  return std::move(drawings);
}

template <bool Threading>
boost::box connection<Threading>::get_mbr(const object& tbl, const column_detail& col)
{
  using namespace boost;

  auto lnk = get_link();
  const std::string sql(detail::sql_mbr(lnk->system(), tbl, col));
  if (sql.empty())
    return box(point(-180, -90), point(180, 90)); // geodetic

  lnk->exec(sql);
  std::vector<variant> row;
  double xmin(0), ymin(0), xmax(0), ymax(0);
  if ( lnk->fetch(row)
    && numeric_cast(row[0], xmin)
    && numeric_cast(row[1], ymin)
    && numeric_cast(row[2], xmax)
    && numeric_cast(row[3], ymax)
     )
    return box(point(xmin, ymin), point(xmax, ymax));

  throw std::runtime_error("mbr error");
}

template <bool Threading>
std::shared_ptr<rowset> connection<Threading>::get_vector_layer
  ( const table_detail<column_detail>& tbl, const std::string& lr, const boost::box& box
  , const std::vector<std::string> cols, int rows
  )
{
  auto lnk = get_link();
  lnk->exec(sql_vector_layer(lnk.get(), tbl, lr, box, cols, rows));
  return lnk;
}

} } // brig::database

#endif // BRIG_DATABASE_CONNECTION_HPP
