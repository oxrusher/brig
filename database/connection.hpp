// Andrew Naplavkov

#ifndef BRIG_DATABASE_CONNECTION_HPP
#define BRIG_DATABASE_CONNECTION_HPP

#include <brig/database/command.hpp>
#include <brig/database/detail/deleter.hpp>
#include <brig/database/detail/link.hpp>
#include <brig/database/detail/linker.hpp>
#include <brig/database/detail/pool.hpp>
#include <brig/database/detail/sql_columns.hpp>
#include <brig/database/detail/sql_indexed_columns.hpp>
#include <brig/database/detail/sql_srid.hpp>
#include <brig/database/detail/sql_tables.hpp>
#include <brig/database/detail/sqlite_table_detail.hpp>
#include <brig/database/global.hpp>
#include <brig/database/index_detail.hpp>
#include <brig/database/numeric_cast.hpp>
#include <brig/database/object.hpp>
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
  void get_tables(std::vector<object>& tables);
  void get_table_detail(const object& tbl, table_detail<column_detail>& meta);
}; // connection

template <bool Threading>
void connection<Threading>::get_tables(std::vector<object>& tables)
{
  using namespace brig::detail;
  auto lnk = get_link();
  lnk->exec(detail::sql_tables(lnk->system()));
  std::vector<variant> row;
  while (lnk->fetch(row))
  {
    object tbl;
    tbl.schema = string_cast<char>(row[0]);
    tbl.name = string_cast<char>(row[1]);
    tables.push_back(tbl);
  }
}

template <bool Threading>
void connection<Threading>::get_table_detail(const object& tbl, table_detail<column_detail>& meta)
{
  using namespace brig::database::detail;
  using namespace brig::detail;

  auto lnk = get_link();
  const DBMS sys(lnk->system());
  if (SQLite == sys)
    sqlite_table_detail(lnk, tbl, meta);
  else
  {
    // columns
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
      meta.cols.push_back(std::move(col));
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
        if (VoidIndex != idx.type) meta.idxs.push_back(std::move(idx));

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

      idx.cols.push_back(string_cast<char>(row[5]));
      int desc(0);
      if (numeric_cast(row[6], desc) && desc) idx.type = VoidIndex;
    }
    if (VoidIndex != idx.type) meta.idxs.push_back(std::move(idx));

    // srid, epsg, extra
    for (size_t i(0); i < meta.cols.size(); ++i)
    {
      const std::string sql(sql_srid(sys, tbl, meta.cols[i]));
      if (!sql.empty())
      {
        lnk->exec(sql);
        if (lnk->fetch(row))
        {
          numeric_cast(row[0], meta.cols[i].srid);
          if (row.size() > 1) numeric_cast(row[1], meta.cols[i].epsg);
          else meta.cols[i].epsg = meta.cols[i].srid;
          if (row.size() > 2) meta.cols[i].type_detail = string_cast<char>(row[2]);
        }
      }
    }
  }
}

} } // brig::database

#endif // BRIG_DATABASE_CONNECTION_HPP
