// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_RASTERS_SQLITE_HPP
#define BRIG_DATABASE_DETAIL_GET_RASTERS_SQLITE_HPP

#include <brig/database/column_definition.hpp>
#include <brig/database/command.hpp>
#include <brig/database/detail/get_rasters.hpp>
#include <brig/database/detail/sql_geometries.hpp>
#include <brig/database/detail/sql_tables.hpp>
#include <brig/database/global.hpp>
#include <brig/database/raster_pyramid.hpp>
#include <memory>
#include <vector>

namespace brig { namespace database { namespace detail {

inline std::vector<raster_pyramid> get_rasters_sqlite(std::shared_ptr<command> cmd)
{
  using namespace brig::database::detail;
  std::vector<variant> row;
  cmd->exec(sql_tables(SQLite, "raster_pyramids"));
  if (cmd->fetch(row))
  {
    cmd->exec("SELECT '', (r.table_prefix || '_rasters') base_tbl, 'raster' base_col, r.pixel_x_size res_x, r.pixel_y_size res_y, '', (r.table_prefix || '_metadata'), 'geometry', 'raster' FROM raster_pyramids r JOIN (" + sql_geometries(SQLite, false) + ") g ON g.tbl = (r.table_prefix || '_metadata') ORDER BY base_tbl, base_col, res_x, res_y");
    auto res = get_rasters(cmd);
    for (size_t r(0); r < res.size(); ++r)
      for (size_t l(0); l < res[r].levels.size(); ++l)
      {
        const std::string tbl(sql_identifier(SQLite, res[r].id));
        const bool hint((double(l) / double(res[r].levels.size())) < 0.15);
        column_definition col;

        res[r].levels[l].raster.query_expression = "(SELECT r FROM (SELECT id i, raster r FROM " + tbl + ") t WHERE t.i = " + "id)";

        col.name = "pixel_x_size";
        col.type = Double;
        col.query_expression = hint? "+pixel_x_size": "";
        col.query_value = res[r].levels[l].resolution.get<0>();
        res[r].levels[l].query_conditions.push_back(col);

        col.name = "pixel_y_size";
        col.type = Double;
        col.query_expression = hint? "+pixel_y_size": "";
        col.query_value = res[r].levels[l].resolution.get<1>();
        res[r].levels[l].query_conditions.push_back(col);
      }
    return res;
  }
  return std::vector<raster_pyramid>();
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_RASTERS_SQLITE_HPP
