// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_RASTER_UNREGISTER_HPP
#define BRIG_DATABASE_DETAIL_SQL_RASTER_UNREGISTER_HPP

#include <brig/database/command.hpp>
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

inline void sql_raster_unregister(std::shared_ptr<command> cmd, const raster_pyramid& raster, std::vector<std::string>& sql)
{
  if (raster.levels.empty()) throw std::runtime_error("raster error");
  for (auto lvl(std::begin(raster.levels)); lvl != std::end(raster.levels); ++lvl)
    if (typeid(std::string) != lvl->raster_column.type() || !lvl->sql_condition.empty()) throw std::runtime_error("raster error");

  const DBMS sys(cmd->system());
  cmd->exec(sql_tables(sys, "simple_rasters"));
  std::vector<variant> row;
  if (!cmd->fetch(row)) throw std::runtime_error("simple_rasters error");
  identifier simple_rasters;
  simple_rasters.schema = string_cast<char>(row[0]);
  simple_rasters.name = string_cast<char>(row[1]);
  if (cmd->fetch(row)) throw std::runtime_error("ambiguous simple_rasters error");

  std::string s;
  s += "DELETE FROM " + sql_identifier(sys, simple_rasters) + " WHERE ";
  if (SQLite != sys) s += sql_identifier(sys, "base_schema") + " = '" + raster.id.schema + "' AND ";
  s += sql_identifier(sys, "base_table") + " = '" + raster.id.name + "' AND " + sql_identifier(sys, "base_raster") + " = '" + raster.id.qualifier + "'";
  sql.push_back(s);
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_RASTER_UNREGISTER_HPP
