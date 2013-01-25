// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_UNREGISTER_HPP
#define BRIG_DATABASE_DETAIL_SQL_UNREGISTER_HPP

#include <brig/database/command.hpp>
#include <brig/database/detail/dialect.hpp>
#include <brig/database/detail/get_table_def.hpp>
#include <brig/database/detail/simple_rasters.hpp>
#include <brig/pyramid_def.hpp>
#include <brig/string_cast.hpp>
#include <brig/table_def.hpp>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

inline void sql_unregister(dialect* dct, command* cmd, const pyramid_def& raster, std::vector<std::string>& sql)
{
  using namespace std;

  cmd->exec("SELECT t.scm, t.tbl FROM (" + dct->sql_tables() + ") t WHERE LOWER(t.tbl) = 'simple_rasters'");
  vector<variant> row;
  if (!cmd->fetch(row)) throw runtime_error("simple_rasters error");
  table_def tbl;
  tbl.id.schema = string_cast<char>(row[0]);
  tbl.id.name = string_cast<char>(row[1]);
  if (cmd->fetch(row)) throw runtime_error("ambiguous simple_rasters error");
  tbl = get_table_def(dct, cmd, tbl.id);
  auto cols(simple_rasters_columns(tbl));

  for (auto lvl(begin(raster.levels)); lvl != end(raster.levels); ++lvl)
  {
    string str;
    str += "DELETE FROM " + dct->sql_identifier(tbl.id) + " WHERE ";
    if (!lvl->geometry.schema.empty()) str += dct->sql_identifier(cols[0]) + " = '" + lvl->geometry.schema + "' AND ";
    str += dct->sql_identifier(cols[1]) + " = '" + lvl->geometry.name + "' AND " + dct->sql_identifier(cols[2]) + " = '" + lvl->raster.name + "'";
    sql.push_back(str);
  }
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_UNREGISTER_HPP
