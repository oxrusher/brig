// Andrew Naplavkov

#ifndef BRIG_DATABASE_CONNECTION_HPP
#define BRIG_DATABASE_CONNECTION_HPP

#include <algorithm>
#include <brig/boost/geometry.hpp>
#include <brig/database/column_definition.hpp>
#include <brig/database/command.hpp>
#include <brig/database/command_allocator.hpp>
#include <brig/database/detail/deleter.hpp>
#include <brig/database/detail/get_raster_layers_postgres.hpp>
#include <brig/database/detail/get_raster_layers_simple.hpp>
#include <brig/database/detail/get_raster_layers_sqlite.hpp>
#include <brig/database/detail/get_table_definition.hpp>
#include <brig/database/detail/get_type.hpp>
#include <brig/database/detail/pool.hpp>
#include <brig/database/detail/sql_create.hpp>
#include <brig/database/detail/sql_drop.hpp>
#include <brig/database/detail/sql_geometry_layers.hpp>
#include <brig/database/detail/sql_insert.hpp>
#include <brig/database/detail/sql_mbr.hpp>
#include <brig/database/detail/sql_register_raster.hpp>
#include <brig/database/detail/sql_schema.hpp>
#include <brig/database/detail/sql_table.hpp>
#include <brig/database/detail/sql_tables.hpp>
#include <brig/database/detail/sql_unregister_raster.hpp>
#include <brig/database/global.hpp>
#include <brig/database/identifier.hpp>
#include <brig/database/index_definition.hpp>
#include <brig/database/numeric_cast.hpp>
#include <brig/database/raster_definition.hpp>
#include <brig/database/rowset.hpp>
#include <brig/database/table_definition.hpp>
#include <brig/database/variant.hpp>
#include <brig/string_cast.hpp>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace brig { namespace database {

template <bool Threading>
class connection {
  typedef detail::pool<Threading> pool_type;
  std::shared_ptr<pool_type> m_pool;

public:
  explicit connection(std::shared_ptr<command_allocator> allocator) : m_pool(new pool_type(allocator))  {}

  std::string get_schema();
  std::vector<identifier> get_tables();
  std::vector<identifier> get_geometry_layers();
  std::vector<raster_definition> get_raster_layers();
  table_definition get_table_definition(const identifier& tbl)  { return detail::get_table_definition(get_command(), tbl); }
  brig::boost::box get_mbr(const identifier& tbl, column_definition& col);

  std::shared_ptr<rowset> get_table(const table_definition& tbl);

  std::shared_ptr<command> get_command()  { return std::shared_ptr<command>(m_pool->allocate(), detail::deleter<pool_type>(m_pool)); }
  void before_create(table_definition& tbl);
  std::vector<std::string> sql_create(table_definition& tbl)  { return detail::sql_create(get_command()->system(), tbl); }
  std::vector<std::string> sql_drop(const table_definition& tbl)  { return detail::sql_drop(get_command()->system(), tbl); }
  std::string sql_insert(const table_definition& tbl, const std::vector<std::string>& cols = std::vector<std::string>())  { return detail::sql_insert(get_command(), tbl, cols); }
  std::vector<std::string> sql_register_raster(const raster_definition& raster)  { return detail::sql_register_raster(get_command(), raster); }
  std::string sql_unregister_raster(const raster_definition& raster)  { return detail::sql_unregister_raster(get_command(), raster); }
}; // connection

template <bool Threading>
std::string connection<Threading>::get_schema()
{
  auto cmd = get_command();
  const std::string sql(detail::sql_schema(cmd->system()));
  if (sql.empty()) return "";
  cmd->exec(sql);
  std::vector<variant> row;
  if (!cmd->fetch(row)) throw std::runtime_error("SQL error");
  return string_cast<char>(row[0]);
}

template <bool Threading>
std::vector<identifier> connection<Threading>::get_tables()
{
  using namespace brig::database::detail;

  std::vector<identifier> res;
  auto cmd = get_command();
  cmd->exec(sql_tables(cmd->system()));
  std::vector<variant> row;
  while (cmd->fetch(row))
  {
    identifier tbl;
    tbl.schema = string_cast<char>(row[0]);
    tbl.name = string_cast<char>(row[1]);
    res.push_back(tbl);
  }
  return res;
}

template <bool Threading>
std::vector<identifier> connection<Threading>::get_geometry_layers()
{
  using namespace brig::database::detail;

  auto cmd = get_command();
  const DBMS sys(cmd->system());
  std::vector<variant> row;
  if (SQLite == sys)
  {
    cmd->exec(sql_tables(sys, "geometry_columns"));
    if (!cmd->fetch(row)) return std::vector<identifier>();
  }

  std::vector<identifier> res;
  cmd->exec(sql_geometry_layers(sys));
  while (cmd->fetch(row))
  {
    identifier col;
    col.schema = string_cast<char>(row[0]);
    col.name = string_cast<char>(row[1]);
    col.qualifier = string_cast<char>(row[2]);
    res.push_back(col);
  }
  return res;
}

template <bool Threading>
std::vector<raster_definition> connection<Threading>::get_raster_layers()
{
  using namespace brig::database::detail;

  auto cmd = get_command();
  std::vector<raster_definition> simple = get_raster_layers_simple(cmd);

  std::vector<raster_definition> specific;
  switch (cmd->system())
  {
  case SQLite: specific = get_raster_layers_sqlite(cmd); break;
  case Postgres: specific = get_raster_layers_postgres(cmd); break;
  }

  std::vector<raster_definition> res;
  auto cmp = [](const raster_definition& a, const raster_definition& b){ return a.id.schema < b.id.schema || a.id.name < b.id.name || a.id.qualifier < b.id.qualifier; };
  std::merge(specific.begin(), specific.end(), simple.begin(), simple.end(), std::back_inserter(res), cmp);
  return res;
}

template <bool Threading>
brig::boost::box connection<Threading>::get_mbr(const identifier& tbl, column_definition& col)
{
  using namespace brig::boost;

  auto cmd = get_command();
  const std::string sql(detail::sql_mbr(cmd->system(), tbl, col));
  if (sql.empty())
  {
    box res(point(-180, -90), point(180, 90)); // geodetic
    col.mbr = res;
    return res;
  }

  cmd->exec(sql);
  std::vector<variant> row;
  double xmin(0), ymin(0), xmax(0), ymax(0);
  if ( cmd->fetch(row)
    && numeric_cast(row[0], xmin)
    && numeric_cast(row[1], ymin)
    && numeric_cast(row[2], xmax)
    && numeric_cast(row[3], ymax)
     )
  {
    box res(point(xmin, ymin), point(xmax, ymax));
    col.mbr = res;
    return res;
  }

  throw std::runtime_error("MBR error");
}

template <bool Threading>
std::shared_ptr<rowset> connection<Threading>::get_table(const table_definition& tbl)
{
  auto cmd = get_command();
  cmd->exec(detail::sql_table(cmd, tbl));
  return cmd;
}

template <bool Threading>
void connection<Threading>::before_create(table_definition& tbl)
{
  auto cmd = get_command();
  const DBMS sys(cmd->system());
  switch (sys)
  {
  case Oracle:
    for (auto p_col = tbl.columns.begin(); p_col != tbl.columns.end(); ++p_col)
      if (Geometry == p_col->type && typeid(bool) == p_col->mbr.type()) p_col->mbr = true;
    break;

  case MS_SQL:
    for (auto p_idx = tbl.indexes.begin(); p_idx != tbl.indexes.end(); ++p_idx)
      if (Spatial == p_idx->type)
      {
        auto p_col = std::find_if(tbl.columns.begin(), tbl.columns.end(), [&](const column_definition& col){ return col.name == p_idx->columns.front(); });
        if (typeid(bool) == p_col->mbr.type()) p_col->mbr = true;
      }
    break;
  }
} // connection::

} } // brig::database

#endif // BRIG_DATABASE_CONNECTION_HPP
