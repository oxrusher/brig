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
#include <brig/database/detail/sql_table.hpp>
#include <brig/database/detail/sql_tables.hpp>
#include <brig/database/global.hpp>
#include <brig/database/identifier.hpp>
#include <brig/database/index_definition.hpp>
#include <brig/database/numeric_cast.hpp>
#include <brig/database/raster_definition.hpp>
#include <brig/database/rowset.hpp>
#include <brig/database/table_definition.hpp>
#include <brig/database/variant.hpp>
#include <brig/detail/string_cast.hpp>
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

  std::vector<identifier> get_tables();
  std::vector<identifier> get_geometry_layers();
  std::vector<raster_definition> get_raster_layers();
  table_definition get_table_definition(const identifier& tbl);
  void get_mbr(const identifier& tbl, column_definition& col);

  std::shared_ptr<rowset> get_table(const table_definition& tbl, const select_options& opts = select_options());

  std::shared_ptr<command> get_command()  { return std::shared_ptr<command>(m_pool->allocate(), detail::deleter<pool_type>(m_pool)); }
  void before_create(table_definition& tbl);
  std::vector<std::string> sql_create(table_definition& tbl);
  std::vector<std::string> sql_drop(const table_definition& tbl)  { return detail::sql_drop(get_command()->system(), tbl); }
  std::string sql_insert(const table_definition& tbl, const std::vector<std::string>& cols = std::vector<std::string>());
}; // connection

template <bool Threading>
std::vector<identifier> connection<Threading>::get_tables()
{
  using namespace brig::database::detail;
  using namespace brig::detail;

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
  using namespace brig::detail;

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
table_definition connection<Threading>::get_table_definition(const identifier& tbl)
{
  auto cmd = get_command();
  return detail::get_table_definition(cmd, tbl);
}

template <bool Threading>
void connection<Threading>::get_mbr(const identifier& tbl, column_definition& col)
{
  using namespace brig::boost;

  auto cmd = get_command();
  const std::string sql(detail::sql_mbr(cmd->system(), tbl, col));
  if (sql.empty())
  {
    col.mbr = box(point(-180, -90), point(180, 90)); // geodetic
    return;
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
    col.mbr = box(point(xmin, ymin), point(xmax, ymax));
    return;
  }

  throw std::runtime_error("mbr error");
}

template <bool Threading>
std::shared_ptr<rowset> connection<Threading>::get_table(const table_definition& tbl, const select_options& opts)
{
  auto cmd = get_command();
  cmd->exec(detail::sql_table(cmd.get(), tbl, opts));
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
}

template <bool Threading>
std::vector<std::string> connection<Threading>::sql_create(table_definition& tbl)
{
  auto cmd = get_command();
  const DBMS sys(cmd->system());
  auto res = detail::sql_create(sys, tbl);

  std::string sql;
  switch (sys)
  {
  case DB2: sql = "VALUES CURRENT_SCHEMA"; break;
  case MySQL: sql = "select schema()"; break;
  case Oracle: sql = "SELECT SYS_CONTEXT('USERENV','SESSION_SCHEMA') FROM DUAL"; break;
  case Postgres: sql = "select current_schema()"; break;
  case MS_SQL: sql = "SELECT SCHEMA_NAME()"; break;
  }

  if (!sql.empty())
  {
    cmd->exec(sql);
    std::vector<variant> row;
    if (cmd->fetch(row)) tbl.id.schema = brig::detail::string_cast<char>(row[0]);
  }

  return res;
}

template <bool Threading>
std::string connection<Threading>::sql_insert(const table_definition& tbl, const std::vector<std::string>& cols)
{
  auto cmd = get_command();
  return detail::sql_insert(cmd.get(), tbl, cols);
} // connection::

} } // brig::database

#endif // BRIG_DATABASE_CONNECTION_HPP
