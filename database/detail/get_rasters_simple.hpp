// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_RASTERS_SIMPLE_HPP
#define BRIG_DATABASE_DETAIL_GET_RASTERS_SIMPLE_HPP

#include <brig/database/command.hpp>
#include <brig/database/detail/get_rasters.hpp>
#include <brig/database/detail/get_table_definition.hpp>
#include <brig/database/detail/simple_rasters.hpp>
#include <brig/database/detail/sql_geometries.hpp>
#include <brig/database/detail/sql_identifier.hpp>
#include <brig/database/detail/sql_tables.hpp>
#include <brig/database/global.hpp>
#include <brig/database/raster_pyramid.hpp>
#include <brig/database/table_definition.hpp>
#include <brig/database/variant.hpp>
#include <brig/string_cast.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

inline std::vector<raster_pyramid> get_rasters_simple(std::shared_ptr<command> cmd)
{
  using namespace std;

  const DBMS sys(cmd->system());
  table_definition tbl;
  cmd->exec(sql_tables(sys, "simple_rasters", false));
  vector<variant> row;
  if (cmd->fetch(row))
  {
    tbl.id.schema = string_cast<char>(row[0]);
    tbl.id.name = string_cast<char>(row[1]);
    if (cmd->fetch(row)) throw runtime_error("ambiguous simple_rasters error");
    tbl = get_table_definition(cmd, tbl.id);
    auto cols = simple_rasters_columns(tbl);

    string sql;
    sql += "SELECT ";
    sql += cols[3].empty()? "''": ("r." + sql_identifier(sys, cols[3]));
    sql += " base_scm, r." + sql_identifier(sys, cols[4]) + " base_tbl, r." + sql_identifier(sys, cols[5]) + " base_col, r." + sql_identifier(sys, cols[7]) + " res_x, r." + sql_identifier(sys, cols[8]) + " res_y, ";
    sql += cols[0].empty()? "''": ("r." + sql_identifier(sys, cols[0]));
    sql += ", r." + sql_identifier(sys, cols[1]) + ", r." + sql_identifier(sys, cols[6]) + ", r." + sql_identifier(sys, cols[2]) + " FROM " + sql_identifier(sys, tbl.id) + " r JOIN (" + sql_geometries(sys, false) + ") g ON ";
    if (!cols[0].empty()) sql += "r." + sql_identifier(sys, cols[0]) + " = g.scm AND ";
    sql += "r." + sql_identifier(sys, cols[1]) + " = g.tbl AND r." + sql_identifier(sys, cols[6]) + " = g.col ORDER BY base_scm, base_tbl, base_col, res_x, res_y";
    cmd->exec(sql);

    return get_rasters(cmd);
  }
  return vector<raster_pyramid>();
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_RASTERS_SIMPLE_HPP
