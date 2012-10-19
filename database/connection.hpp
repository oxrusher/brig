// Andrew Naplavkov

#ifndef BRIG_DATABASE_CONNECTION_HPP
#define BRIG_DATABASE_CONNECTION_HPP

#include <algorithm>
#include <brig/boost/geometry.hpp>
#include <brig/database/command_allocator.hpp>
#include <brig/database/detail/fit_raster.hpp>
#include <brig/database/detail/fit_table.hpp>
#include <brig/database/detail/get_rasters_postgres.hpp>
#include <brig/database/detail/get_rasters_simple.hpp>
#include <brig/database/detail/get_rasters_sqlite.hpp>
#include <brig/database/detail/get_schema.hpp>
#include <brig/database/detail/get_srid.hpp>
#include <brig/database/detail/get_table_definition.hpp>
#include <brig/database/detail/pool.hpp>
#include <brig/database/detail/sql_create.hpp>
#include <brig/database/detail/sql_drop.hpp>
#include <brig/database/detail/sql_geometries.hpp>
#include <brig/database/detail/sql_insert.hpp>
#include <brig/database/detail/sql_mbr.hpp>
#include <brig/database/detail/sql_raster_register.hpp>
#include <brig/database/detail/sql_raster_unregister.hpp>
#include <brig/database/detail/sql_select.hpp>
#include <brig/database/detail/sql_tables.hpp>
#include <brig/database/global.hpp>
#include <brig/database/numeric_cast.hpp>
#include <brig/database/raster_pyramid.hpp>
#include <brig/database/rowset.hpp>
#include <brig/database/table_definition.hpp>
#include <brig/database/variant.hpp>
#include <brig/detail/deleter.hpp>
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

  std::vector<identifier> get_tables();
  std::vector<identifier> get_geometry_layers();
  std::vector<raster_pyramid> get_raster_layers();
  table_definition get_table_definition(const identifier& tbl)  { return detail::get_table_definition(get_command(), tbl); }
  brig::boost::box get_mbr(const identifier& tbl, const column_definition& col);
  std::shared_ptr<command> get_command()  { return std::shared_ptr<command>(m_pool->allocate(), brig::detail::deleter<command, pool_type>(m_pool)); }

  std::shared_ptr<rowset> select(const table_definition& tbl);
  std::string insert(const table_definition& tbl)  { return detail::sql_insert(get_command(), tbl); }

  /**
  AFTER CALL: define bounding box with boost::as_binary if column_definition.query_value is empty blob_t
  */
  table_definition fit_to_create(const table_definition& tbl);
  void create(const table_definition& tbl, std::vector<std::string>& sql)  { detail::sql_create(get_command()->system(), tbl, sql); }
  void drop(const table_definition& tbl, std::vector<std::string>& sql)  { detail::sql_drop(get_command()->system(), tbl, sql); }
  
  raster_pyramid fit_to_reg(const raster_pyramid& raster);
  void reg(const raster_pyramid& raster, std::vector<std::string>& sql)  { detail::sql_raster_register(get_command(), raster, sql); }
  void unreg(const raster_pyramid& raster, std::vector<std::string>& sql)  { detail::sql_raster_unregister(get_command(), raster, sql); }
}; // connection

template <bool Threading>
std::vector<identifier> connection<Threading>::get_tables()
{
  using namespace std;
  using namespace brig::database::detail;

  vector<identifier> res;
  auto cmd(get_command());
  cmd->exec(sql_tables(cmd->system(), string(), true));
  vector<variant> row;
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
  using namespace std;
  using namespace brig::database::detail;

  vector<identifier> res;
  auto cmd(get_command());
  const DBMS sys(cmd->system());
  vector<variant> row;

  switch (sys)
  {
    default: break;
    case CUBRID: return res;
    case SQLite:
      cmd->exec(sql_tables(sys, "geometry_columns", false));
      if (!cmd->fetch(row)) return res;
      break;
  }
  
  cmd->exec(sql_geometries(sys, true));
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
std::vector<raster_pyramid> connection<Threading>::get_raster_layers()
{
  using namespace std;
  using namespace brig::database::detail;

  auto cmd(get_command());
  vector<raster_pyramid> simple(get_rasters_simple(cmd));

  vector<raster_pyramid> specific;
  switch (cmd->system())
  {
  default: break;
  case SQLite: specific = get_rasters_sqlite(cmd); break;
  case Postgres: specific = get_rasters_postgres(cmd); break;
  }

  vector<raster_pyramid> res;
  auto cmp([](const raster_pyramid& a, const raster_pyramid& b){ return a.id.schema < b.id.schema || a.id.name < b.id.name || a.id.qualifier < b.id.qualifier; });
  merge(begin(specific), end(specific), begin(simple), end(simple), back_inserter(res), cmp);
  return res;
}

template <bool Threading>
brig::boost::box connection<Threading>::get_mbr(const identifier& tbl, const column_definition& col)
{
  using namespace std;
  using namespace brig::boost;

  auto cmd(get_command());
  const string sql(detail::sql_mbr(cmd->system(), tbl, col));
  if (sql.empty()) return box(point(-180, -90), point(180, 90)); // geodetic

  cmd->exec(sql);
  vector<variant> row;
  double xmin(0), ymin(0), xmax(0), ymax(0);
  if ( cmd->fetch(row)
    && numeric_cast(row[0], xmin)
    && numeric_cast(row[1], ymin)
    && numeric_cast(row[2], xmax)
    && numeric_cast(row[3], ymax)
     )
    return box(point(xmin, ymin), point(xmax, ymax));

  throw runtime_error("MBR error");
}

template <bool Threading>
std::shared_ptr<rowset> connection<Threading>::select(const table_definition& tbl)
{
  using namespace std;

  auto cmd(get_command());
  string sql;
  vector<column_definition> params;
  detail::sql_select(cmd, tbl, sql, params);
  cmd->exec(sql, params);
  return cmd;
}

template <bool Threading>
table_definition connection<Threading>::fit_to_create(const table_definition& tbl)
{
  using namespace std;

  auto cmd(get_command());
  table_definition res(detail::fit_table(tbl, cmd->system(), detail::get_schema(cmd)));
  for (auto col(begin(res.columns)); col != end(res.columns); ++col)
    if (Geometry == col->type)
      col->srid = detail::get_srid(cmd, col->epsg);
  return res;
}

template <bool Threading>
raster_pyramid connection<Threading>::fit_to_reg(const raster_pyramid& raster)
{
  auto cmd(get_command());
  return detail::fit_raster(raster, cmd->system(), detail::get_schema(cmd));
} // connection::

} } // brig::database

#endif // BRIG_DATABASE_CONNECTION_HPP
