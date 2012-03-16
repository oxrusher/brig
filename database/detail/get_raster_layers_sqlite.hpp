// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_RASTER_LAYERS_SQLITE_HPP
#define BRIG_DATABASE_DETAIL_GET_RASTER_LAYERS_SQLITE_HPP

#include <brig/database/column_definition.hpp>
#include <brig/database/command.hpp>
#include <brig/database/detail/get_raster_layers.hpp>
#include <brig/database/detail/sql_geometry_layers.hpp>
#include <brig/database/detail/sql_tables.hpp>
#include <brig/database/global.hpp>
#include <brig/database/raster_definition.hpp>
#include <brig/database/variant.hpp>
#include <brig/unicode/lower_case.hpp>
#include <brig/unicode/transform.hpp>
#include <ios>
#include <locale>
#include <memory>
#include <sstream>
#include <vector>

namespace brig { namespace database { namespace detail {

inline std::vector<raster_definition> get_raster_layers_sqlite(std::shared_ptr<command> cmd)
{
  using namespace brig::database::detail;
  using namespace brig::unicode;
  std::vector<variant> row;
  cmd->exec(sql_tables(SQLite, "raster_pyramids"));
  if (cmd->fetch(row))
  {
    cmd->exec("SELECT '', (r.table_prefix || '_rasters') base_tbl, 'raster' base_col, r.pixel_x_size res_x, r.pixel_y_size res_y, '', (r.table_prefix || '_metadata'), 'geometry', 'raster' FROM raster_pyramids r JOIN (" + sql_geometry_layers(SQLite) + ") g ON g.tbl = (r.table_prefix || '_metadata') ORDER BY base_tbl, base_col, res_x, res_y");
    auto res = get_raster_layers(cmd);
    for (size_t r(0); r < res.size(); ++r)
      for (size_t l(0); l < res[r].levels.size(); ++l)
      {
        const std::string tbl(sql_identifier(SQLite, res[r].id));
        const std::string hint((double(l) / double(res[r].levels.size())) < 0.15? "+": "");

        column_definition col;
        col.name = ::boost::get<std::string>(res[r].levels[l].raster_column);
        col.sql_expression = "(SELECT r FROM (SELECT id i, raster r FROM " + tbl + ") t WHERE t.i = " + "id)";
        col.dbms_type.name = "BLOB";
        col.lower_case_type.name = transform<std::string>(col.dbms_type.name, lower_case);
        col.type = Blob;
        res[r].levels[l].raster_column = col;

        std::ostringstream stream; stream.imbue(std::locale::classic()); stream << std::scientific; stream.precision(16);
        stream << "pixel_x_size = " << res[r].levels[l].resolution.get<0>() << " AND " + hint + "pixel_y_size = " << res[r].levels[l].resolution.get<1>();
        res[r].levels[l].sql_filter = stream.str();
      }
    return res;
  }
  return std::vector<raster_definition>();
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_RASTER_LAYERS_SQLITE_HPP
