// Andrew Naplavkov

#ifndef BRIG_DATABASE_CONNECTION_HPP
#define BRIG_DATABASE_CONNECTION_HPP

#include <brig/boost/geometry.hpp>
#include <brig/database/command_allocator.hpp>
#include <brig/database/detail/dialect_factory.hpp>
#include <brig/database/detail/fit_raster.hpp>
#include <brig/database/detail/get_geometry_layers.hpp>
#include <brig/database/detail/get_mbr.hpp>
#include <brig/database/detail/get_raster_layers.hpp>
#include <brig/database/detail/get_schema.hpp>
#include <brig/database/detail/get_srid.hpp>
#include <brig/database/detail/get_table_definition.hpp>
#include <brig/database/detail/get_tables.hpp>
#include <brig/database/detail/pool.hpp>
#include <brig/database/detail/sql_create.hpp>
#include <brig/database/detail/sql_drop.hpp>
#include <brig/database/detail/sql_insert.hpp>
#include <brig/database/detail/sql_register.hpp>
#include <brig/database/detail/sql_select.hpp>
#include <brig/database/detail/sql_unregister.hpp>
#include <brig/database/global.hpp>
#include <brig/database/raster_pyramid.hpp>
#include <brig/database/table_definition.hpp>
#include <brig/detail/deleter.hpp>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

namespace brig { namespace database {

template <bool Threading>
class connection {
  typedef detail::pool<Threading> pool_type;
  std::shared_ptr<pool_type> m_pool;
  std::shared_ptr<detail::dialect> m_dct;

public:
  explicit connection(std::shared_ptr<command_allocator> allocator);

  std::vector<identifier> get_tables();
  std::vector<identifier> get_geometry_layers();
  std::vector<raster_pyramid> get_raster_layers();

  table_definition get_table_definition(const identifier& tbl);

  brig::boost::box get_mbr(const table_definition& tbl, const std::string& col);

  std::shared_ptr<command> get_command();

  std::shared_ptr<rowset> select(const table_definition& tbl);
  std::string insert(const table_definition& tbl);

  /**
  AFTER CALL: define bounding box with boost::as_binary if column_definition.query_value is empty blob_t
  */
  table_definition fit_to_create(const table_definition& tbl);
  void create(const table_definition& tbl, std::vector<std::string>& sql);
  void drop(const table_definition& tbl, std::vector<std::string>& sql);
  
  raster_pyramid fit_to_reg(const raster_pyramid& raster);
  void reg(const raster_pyramid& raster, std::vector<std::string>& sql);
  void unreg(const raster_pyramid& raster, std::vector<std::string>& sql);
}; // connection

template <bool Threading>
connection<Threading>::connection(std::shared_ptr<command_allocator> allocator) : m_pool(new pool_type(allocator))
{
  auto cmd(get_command());
  m_dct = std::shared_ptr<detail::dialect>(detail::dialect_factory(cmd->system()));
}

template <bool Threading>
std::shared_ptr<command> connection<Threading>::get_command()
{
  return std::shared_ptr<command>(m_pool->allocate(), brig::detail::deleter<command, pool_type>(m_pool));
}

template <bool Threading>
std::vector<identifier> connection<Threading>::get_tables()
{
  auto cmd(get_command());
  return detail::get_tables(m_dct.get(), cmd.get());
  return std::vector<identifier>();
}

template <bool Threading>
std::vector<identifier> connection<Threading>::get_geometry_layers()
{
  auto cmd(get_command());
  return detail::get_geometry_layers(m_dct.get(), cmd.get());
}

template <bool Threading>
table_definition connection<Threading>::get_table_definition(const identifier& tbl)
{
  auto cmd(get_command());
  return detail::get_table_definition(m_dct.get(), cmd.get(), tbl);
}

template <bool Threading>
brig::boost::box connection<Threading>::get_mbr(const table_definition& tbl, const std::string& col)
{
  auto cmd(get_command());
  return detail::get_mbr(m_dct.get(), cmd.get(), tbl, col);
}

template <bool Threading>
table_definition connection<Threading>::fit_to_create(const table_definition& tbl)
{
  using namespace std;

  auto cmd(get_command());
  table_definition res(m_dct->fit_table(tbl, detail::get_schema(m_dct.get(), cmd.get())));
  for (auto col(begin(res.columns)); col != end(res.columns); ++col)
    if (col->epsg >= 0)
      col->srid = detail::get_srid(m_dct.get(), cmd.get(), col->epsg);
  return res;
}

template <bool Threading>
void connection<Threading>::create(const table_definition& tbl, std::vector<std::string>& sql)
{
  detail::sql_create(m_dct.get(), tbl, sql);
}

template <bool Threading>
void connection<Threading>::drop(const table_definition& tbl, std::vector<std::string>& sql)
{
  detail::sql_drop(m_dct.get(), tbl, sql);
}

template <bool Threading>
std::vector<raster_pyramid> connection<Threading>::get_raster_layers()
{
  auto cmd(get_command());
  return detail::get_raster_layers(m_dct.get(), cmd.get());
}

template <bool Threading>
raster_pyramid connection<Threading>::fit_to_reg(const raster_pyramid& raster)
{
  auto cmd(get_command());
  return detail::fit_raster(m_dct.get(), raster, detail::get_schema(m_dct.get(), cmd.get()));
}

template <bool Threading>
void connection<Threading>::reg(const raster_pyramid& raster, std::vector<std::string>& sql)
{
  auto cmd(get_command());
  detail::sql_register(m_dct.get(), cmd.get(), raster, sql);
}

template <bool Threading>
void connection<Threading>::unreg(const raster_pyramid& raster, std::vector<std::string>& sql)
{
  auto cmd(get_command());
  detail::sql_unregister(m_dct.get(), cmd.get(), raster, sql);
}

template <bool Threading>
std::shared_ptr<rowset> connection<Threading>::select(const table_definition& tbl)
{
  using namespace std;

  auto cmd(get_command());
  string sql;
  vector<column_definition> params;
  detail::sql_select(m_dct.get(), cmd->traits(), tbl, sql, params);
  cmd->exec(sql, params);
  return cmd;
}

template <bool Threading>
std::string connection<Threading>::insert(const table_definition& tbl)
{
  auto cmd(get_command());
  return detail::sql_insert(m_dct.get(), cmd->traits(), tbl);
} // connection::

} } // brig::database

#endif // BRIG_DATABASE_CONNECTION_HPP
