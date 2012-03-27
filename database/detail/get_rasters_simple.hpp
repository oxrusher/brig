// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_RASTERS_SIMPLE_HPP
#define BRIG_DATABASE_DETAIL_GET_RASTERS_SIMPLE_HPP

#include <brig/database/command.hpp>
#include <brig/database/detail/get_rasters.hpp>
#include <brig/database/detail/sql_geometries.hpp>
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
#include <vector>

namespace brig { namespace database { namespace detail {

inline std::vector<raster_pyramid> get_rasters_simple(std::shared_ptr<command> cmd)
{
  const DBMS sys(cmd->system());
  cmd->exec(sql_tables(sys, "simple_rasters"));
  std::vector<variant> row;
  if (cmd->fetch(row))
  {
    identifier simple_rasters;
    simple_rasters.schema = string_cast<char>(row[0]);
    simple_rasters.name = string_cast<char>(row[1]);
    if (cmd->fetch(row)) throw std::runtime_error("ambiguous simple_rasters error");

    std::string sql;
    sql += "SELECT ";
    sql += (SQLite == sys? "''": "r." + sql_identifier(sys, "base_schema"));
    sql += " base_scm, r." + sql_identifier(sys, "base_table") + " base_tbl, r." + sql_identifier(sys, "base_raster") + " base_col, r." + sql_identifier(sys, "resolution_x") + " res_x, r." + sql_identifier(sys, "resolution_y") + " res_y, ";
    sql += SQLite == sys? "''": ("r." + sql_identifier(sys, "schema"));
    sql += ", r." + sql_identifier(sys, "table") + ", r." + sql_identifier(sys, "geometry") + ", r." + sql_identifier(sys, "raster") + " FROM " + sql_identifier(sys, simple_rasters) + " r JOIN (" + sql_geometries(sys, false) + ") g ON ";
    if (SQLite != sys) sql += "r." + sql_identifier(sys, "schema") + " = g.scm AND ";
    sql += "r." + sql_identifier(sys, "table") + " = g.tbl AND r." + sql_identifier(sys, "geometry") + " = g.col ORDER BY base_scm, base_tbl, base_col, res_x, res_y";

    cmd->exec(sql);
    return get_rasters(cmd);
  }

  return std::vector<raster_pyramid>();
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_RASTERS_SIMPLE_HPP
