// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_UNREGISTER_RASTER_HPP
#define BRIG_DATABASE_DETAIL_SQL_UNREGISTER_RASTER_HPP

#include <brig/database/command.hpp>
#include <brig/database/detail/sql_identifier.hpp>
#include <brig/database/detail/sql_tables.hpp>
#include <brig/database/global.hpp>
#include <brig/database/identifier.hpp>
#include <brig/database/raster_definition.hpp>
#include <brig/database/variant.hpp>
#include <brig/detail/string_cast.hpp>
#include <memory>
#include <stdexcept>
#include <string>

namespace brig { namespace database { namespace detail {

inline std::string sql_unregister_raster(std::shared_ptr<command> cmd, const raster_definition& raster)
{
  using namespace brig::detail;

  const DBMS sys(cmd->system());
  cmd->exec(sql_tables(sys, "simple_rasters"));
  std::vector<variant> row;
  if (!cmd->fetch(row)) throw std::runtime_error("sql error");

  identifier id;
  id.schema = string_cast<char>(row[0]);
  id.name = string_cast<char>(row[1]);

  std::string sql;
  sql += "DELETE FROM " + sql_identifier(sys, id) + " WHERE ";
  if (SQLite != sys) sql += sql_identifier(sys, "base_schema") + " = '" + raster.id.schema + "' AND ";
  sql += sql_identifier(sys, "base_table") + " = '" + raster.id.name + "' AND " + sql_identifier(sys, "base_raster") + " = '" + raster.id.qualifier + "'";
  return sql;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_UNREGISTER_RASTER_HPP
