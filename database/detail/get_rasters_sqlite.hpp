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
#include <brig/database/variant.hpp>
#include <brig/unicode/lower_case.hpp>
#include <brig/unicode/transform.hpp>
#include <ios>
#include <memory>
#include <vector>

namespace brig { namespace database { namespace detail {

inline std::vector<raster_pyramid> get_rasters_sqlite(std::shared_ptr<command> cmd)
{
  using namespace brig::database::detail;
  using namespace brig::unicode;
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
        const std::string hint((double(l) / double(res[r].levels.size())) < 0.15? "+": "");

        column_definition col;
        col.name = ::boost::get<std::string>(res[r].levels[l].raster_column);
        col.sql_expression = "(SELECT r FROM (SELECT id i, raster r FROM " + tbl + ") t WHERE t.i = " + "id)";
        col.dbms_type.name = "BLOB";
        col.dbms_type_lcase.name = transform<std::string>(col.dbms_type.name, lower_case);
        col.type = Blob;
        res[r].levels[l].raster_column = col;
        res[r].levels[l].sql_condition = hint + "pixel_x_size = " + cmd->sql_parameter(0, column_definition()) + " AND " + hint + "pixel_y_size = " + cmd->sql_parameter(1, column_definition());
        res[r].levels[l].parameters.push_back( res[r].levels[l].resolution.get<0>() );
        res[r].levels[l].parameters.push_back( res[r].levels[l].resolution.get<1>() );
      }
    return res;
  }
  return std::vector<raster_pyramid>();
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_RASTERS_SQLITE_HPP
