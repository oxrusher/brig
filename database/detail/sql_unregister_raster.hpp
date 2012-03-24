// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_UNREGISTER_RASTER_HPP
#define BRIG_DATABASE_DETAIL_SQL_UNREGISTER_RASTER_HPP

#include <brig/database/command.hpp>
#include <brig/database/detail/normalize_identifier.hpp>
#include <brig/database/detail/sql_identifier.hpp>
#include <brig/database/detail/sql_tables.hpp>
#include <brig/database/global.hpp>
#include <brig/database/identifier.hpp>
#include <brig/database/raster_pyramid.hpp>
#include <brig/database/variant.hpp>
#include <brig/string_cast.hpp>
#include <memory>
#include <stdexcept>
#include <string>

namespace brig { namespace database { namespace detail {

inline void sql_unregister_raster(std::shared_ptr<command> cmd, const raster_pyramid& raster, std::vector<std::string>& sql)
{
  const DBMS sys(cmd->system());
  identifier simple_rasters;
  simple_rasters.name = "simple_rasters";
  normalize_identifier(sys, simple_rasters);
  cmd->exec(sql_tables(sys, simple_rasters.name));
  std::vector<variant> row;
  if (!cmd->fetch(row)) throw std::runtime_error("simple_rasters error");
  simple_rasters.schema = string_cast<char>(row[0]);

  if (cmd->fetch(row)) throw std::runtime_error("ambiguous simple_rasters error");

  std::string s;
  s += "DELETE FROM " + sql_identifier(sys, simple_rasters) + " WHERE ";
  if (SQLite != sys) s += sql_identifier(sys, "base_schema") + " = '" + raster.id.schema + "' AND ";
  s += sql_identifier(sys, "base_table") + " = '" + raster.id.name + "' AND " + sql_identifier(sys, "base_raster") + " = '" + raster.id.qualifier + "'";
  sql.push_back(s);
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_UNREGISTER_RASTER_HPP
