// Andrew Naplavkov

#ifndef BRIG_DATABASE_CONNECTION_HPP
#define BRIG_DATABASE_CONNECTION_HPP

#include <brig/connection.hpp>
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
#include <brig/database/detail/inserter.hpp>
#include <brig/database/detail/pool.hpp>
#include <brig/database/detail/sql_create.hpp>
#include <brig/database/detail/sql_drop.hpp>
#include <brig/database/detail/sql_register.hpp>
#include <brig/database/detail/sql_select.hpp>
#include <brig/database/detail/sql_unregister.hpp>
#include <brig/detail/deleter.hpp>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

namespace brig { namespace database {

template <bool Threading>
class connection : public brig::connection {
  typedef detail::pool<Threading> pool_t;
  typedef brig::detail::deleter<command, pool_t> deleter_t;
  std::shared_ptr<pool_t> m_pool;

public:
  explicit connection(std::shared_ptr<command_allocator> allocator) : m_pool(new pool_t(allocator))  {}

  std::vector<identifier> get_tables() override;
  std::vector<identifier> get_geometry_layers() override;
  std::vector<raster_pyramid> get_raster_layers() override;
  table_definition get_table_definition(const identifier& tbl) override;

  brig::boost::box get_mbr(const table_definition& tbl, const std::string& col) override;
  std::shared_ptr<rowset> select(const table_definition& tbl) override;

  /*!
  AFTER CALL: define bounding box with boost::as_binary() if column_definition.query_value is empty blob_t
  */
  table_definition fit_to_create(const table_definition& tbl) override;
  void create(const table_definition& tbl) override;
  void drop(const table_definition& tbl) override;
  
  raster_pyramid fit_to_reg(const raster_pyramid& raster) override;
  void reg(const raster_pyramid& raster) override;
  void unreg(const raster_pyramid& raster) override;

  std::shared_ptr<inserter> get_inserter(const table_definition& tbl) override;

  std::shared_ptr<command> get_command();
  void create(const table_definition& tbl, std::vector<std::string>& sql);
  void reg(const raster_pyramid& raster, std::vector<std::string>& sql);
}; // connection

template <bool Threading>
std::shared_ptr<command> connection<Threading>::get_command()
{
  return std::shared_ptr<command>(m_pool->allocate(), deleter_t(m_pool));
}

template <bool Threading>
std::vector<identifier> connection<Threading>::get_tables()
{
  using namespace std;
  using namespace detail;
  unique_ptr<command, deleter_t> cmd(m_pool->allocate(), deleter_t(m_pool));
  unique_ptr<dialect> dct(dialect_factory(cmd->system()));
  return detail::get_tables(dct.get(), cmd.get());
}

template <bool Threading>
std::vector<identifier> connection<Threading>::get_geometry_layers()
{
  using namespace std;
  using namespace detail;
  unique_ptr<command, deleter_t> cmd(m_pool->allocate(), deleter_t(m_pool));
  unique_ptr<dialect> dct(dialect_factory(cmd->system()));
  return detail::get_geometry_layers(dct.get(), cmd.get());
}

template <bool Threading>
table_definition connection<Threading>::get_table_definition(const identifier& tbl)
{
  using namespace std;
  using namespace detail;
  unique_ptr<command, deleter_t> cmd(m_pool->allocate(), deleter_t(m_pool));
  unique_ptr<dialect> dct(dialect_factory(cmd->system()));
  return detail::get_table_definition(dct.get(), cmd.get(), tbl);
}

template <bool Threading>
brig::boost::box connection<Threading>::get_mbr(const table_definition& tbl, const std::string& col)
{
  using namespace std;
  using namespace detail;
  unique_ptr<command, deleter_t> cmd(m_pool->allocate(), deleter_t(m_pool));
  unique_ptr<dialect> dct(dialect_factory(cmd->system()));
  return detail::get_mbr(dct.get(), cmd.get(), tbl, col);
}

template <bool Threading>
table_definition connection<Threading>::fit_to_create(const table_definition& tbl)
{
  using namespace std;
  using namespace detail;
  unique_ptr<command, deleter_t> cmd(m_pool->allocate(), deleter_t(m_pool));
  unique_ptr<dialect> dct(dialect_factory(cmd->system()));
  table_definition res(dct->fit_table(tbl, get_schema(dct.get(), cmd.get())));
  for (auto col(begin(res.columns)); col != end(res.columns); ++col)
    if (col->epsg >= 0)
      col->srid = get_srid(dct.get(), cmd.get(), col->epsg);
  return res;
}

template <bool Threading>
void connection<Threading>::create(const table_definition& tbl, std::vector<std::string>& sql)
{
  using namespace std;
  using namespace detail;
  unique_ptr<command, deleter_t> cmd(m_pool->allocate(), deleter_t(m_pool));
  unique_ptr<dialect> dct(dialect_factory(cmd->system()));
  sql_create(dct.get(), tbl, sql);
}

template <bool Threading>
void connection<Threading>::create(const table_definition& tbl)
{
  using namespace std;
  using namespace detail;
  unique_ptr<command, deleter_t> cmd(m_pool->allocate(), deleter_t(m_pool));
  unique_ptr<dialect> dct(dialect_factory(cmd->system()));
  vector<string> sql;
  sql_create(dct.get(), tbl, sql);
  for (auto str(begin(sql)); str != end(sql); ++str)
    cmd->exec(*str);
}

template <bool Threading>
void connection<Threading>::drop(const table_definition& tbl)
{
  using namespace std;
  using namespace detail;
  unique_ptr<command, deleter_t> cmd(m_pool->allocate(), deleter_t(m_pool));
  unique_ptr<dialect> dct(dialect_factory(cmd->system()));
  vector<string> sql;
  sql_drop(dct.get(), tbl, sql);
  for (auto str(begin(sql)); str != end(sql); ++str)
    cmd->exec(*str);
}

template <bool Threading>
std::vector<raster_pyramid> connection<Threading>::get_raster_layers()
{
  using namespace std;
  using namespace detail;
  unique_ptr<command, deleter_t> cmd(m_pool->allocate(), deleter_t(m_pool));
  unique_ptr<dialect> dct(dialect_factory(cmd->system()));
  return detail::get_raster_layers(dct.get(), cmd.get());
}

template <bool Threading>
raster_pyramid connection<Threading>::fit_to_reg(const raster_pyramid& raster)
{
  using namespace std;
  using namespace detail;
  unique_ptr<command, deleter_t> cmd(m_pool->allocate(), deleter_t(m_pool));
  unique_ptr<dialect> dct(dialect_factory(cmd->system()));
  return fit_raster(dct.get(), raster, get_schema(dct.get(), cmd.get()));
}

template <bool Threading>
void connection<Threading>::reg(const raster_pyramid& raster, std::vector<std::string>& sql)
{
  using namespace std;
  using namespace detail;
  unique_ptr<command, deleter_t> cmd(m_pool->allocate(), deleter_t(m_pool));
  unique_ptr<dialect> dct(dialect_factory(cmd->system()));
  sql_register(dct.get(), cmd.get(), raster, sql);
}

template <bool Threading>
void connection<Threading>::reg(const raster_pyramid& raster)
{
  using namespace std;
  using namespace detail;
  unique_ptr<command, deleter_t> cmd(m_pool->allocate(), deleter_t(m_pool));
  unique_ptr<dialect> dct(dialect_factory(cmd->system()));
  vector<string> sql;
  sql_register(dct.get(), cmd.get(), raster, sql);
  for (auto str(begin(sql)); str != end(sql); ++str)
    cmd->exec(*str);
}

template <bool Threading>
void connection<Threading>::unreg(const raster_pyramid& raster)
{
  using namespace std;
  using namespace detail;
  unique_ptr<command, deleter_t> cmd(m_pool->allocate(), deleter_t(m_pool));
  unique_ptr<dialect> dct(dialect_factory(cmd->system()));
  vector<string> sql;
  sql_unregister(dct.get(), cmd.get(), raster, sql);
  for (auto str(begin(sql)); str != end(sql); ++str)
    cmd->exec(*str);
}

template <bool Threading>
std::shared_ptr<rowset> connection<Threading>::select(const table_definition& tbl)
{
  using namespace std;
  using namespace detail;
  auto cmd(get_command());
  unique_ptr<dialect> dct(dialect_factory(cmd->system()));
  string sql;
  vector<column_definition> params;
  sql_select(dct.get(), cmd->traits(), tbl, sql, params);
  cmd->exec(sql, params);
  return cmd;
}

template <bool Threading>
std::shared_ptr<inserter> connection<Threading>::get_inserter(const table_definition& tbl)
{
  return std::shared_ptr<inserter>(new detail::inserter<deleter_t>(m_pool->allocate(), deleter_t(m_pool), tbl));
} // connection::

} } // brig::database

#endif // BRIG_DATABASE_CONNECTION_HPP
