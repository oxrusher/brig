// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_RASTER_LAYERS_HPP
#define BRIG_DATABASE_DETAIL_GET_RASTER_LAYERS_HPP

#include <brig/database/command.hpp>
#include <brig/database/detail/dialect.hpp>
#include <brig/database/detail/get_table_def.hpp>
#include <brig/database/detail/simple_rasters.hpp>
#include <brig/global.hpp>
#include <brig/pyramid_def.hpp>
#include <brig/string_cast.hpp>
#include <iterator>
#include <vector>

namespace brig { namespace database { namespace detail {

inline std::vector<pyramid_def> get_rasters(rowset* rs)
{
  using namespace std;

  vector<pyramid_def> res;
  vector<variant> row;
  identifier prev_id;
  while (rs->fetch(row))
  {
    identifier cur_id = { string_cast<char>(row[0]), string_cast<char>(row[1]), string_cast<char>(row[2]) };
    if (cur_id.schema != prev_id.schema || cur_id.name != prev_id.name || cur_id.qualifier != prev_id.qualifier)
    {
      res.push_back(pyramid_def());
      res.back().id = cur_id;
      prev_id = cur_id;
    }

    tilemap_def lvl;
    numeric_cast(row[3], lvl.resolution_x);
    numeric_cast(row[4], lvl.resolution_y);
    lvl.geometry.schema = string_cast<char>(row[5]);
    lvl.geometry.name = string_cast<char>(row[6]);
    lvl.geometry.qualifier = string_cast<char>(row[7]);
    lvl.raster.name = string_cast<char>(row[8]);
    lvl.raster.type = column_type::Blob;
    res.back().levels.push_back(lvl);
  }
  return res;
}

inline std::vector<pyramid_def> get_raster_layers(dialect* dct, command* cmd)
{
  using namespace std;

  vector<pyramid_def> native;
  string sql(dct->sql_test_rasters());
  if (!sql.empty())
  {
    cmd->exec(sql);
    vector<variant> row;
    if (cmd->fetch(row))
    {
      cmd->exec(dct->sql_rasters());
      native = get_rasters(cmd);
      for (auto& raster: native)
        dct->init_raster(raster);
    }
  }

  vector<pyramid_def> simple;
  cmd->exec("SELECT t.scm, t.tbl FROM (" + dct->sql_tables() + ") t WHERE LOWER(t.tbl) = 'simple_rasters'");
  vector<variant> row;
  if (cmd->fetch(row))
  {
    table_def tbl;
    tbl.id.schema = string_cast<char>(row[0]);
    tbl.id.name = string_cast<char>(row[1]);
    if (cmd->fetch(row)) throw runtime_error("ambiguous simple_rasters error");
    
    tbl = get_table_def(dct, cmd, tbl.id);
    auto cols = simple_rasters_columns(tbl);

    sql.clear();
    sql += "SELECT ";
    sql += cols[3].empty()? "''": ("r." + dct->sql_identifier(cols[3]));
    sql += " base_scm, r." + dct->sql_identifier(cols[4]) + " base_tbl, r." + dct->sql_identifier(cols[5]) + " base_col, r." + dct->sql_identifier(cols[7]) + " res_x, r." + dct->sql_identifier(cols[8]) + " res_y, ";
    sql += cols[0].empty()? "''": ("r." + dct->sql_identifier(cols[0]));
    sql += ", r." + dct->sql_identifier(cols[1]) + ", r." + dct->sql_identifier(cols[6]) + ", r." + dct->sql_identifier(cols[2]) + " FROM " + dct->sql_identifier(tbl.id) + " r JOIN (" + dct->sql_geometries() + ") g ON ";
    if (!cols[0].empty()) sql += "r." + dct->sql_identifier(cols[0]) + " = g.scm AND ";
    sql += "r." + dct->sql_identifier(cols[1]) + " = g.tbl AND r." + dct->sql_identifier(cols[6]) + " = g.col ORDER BY base_scm, base_tbl, base_col, res_x, res_y";
    cmd->exec(sql);
    simple = get_rasters(cmd);
  }

  vector<pyramid_def> res;
  auto cmp([](const pyramid_def& a, const pyramid_def& b){ return a.id.schema < b.id.schema || a.id.name < b.id.name || a.id.qualifier < b.id.qualifier; });
  merge(begin(native), end(native), begin(simple), end(simple), back_inserter(res), cmp);
  return res;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_RASTER_LAYERS_HPP
