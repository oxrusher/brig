// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_REGISTER_HPP
#define BRIG_DATABASE_DETAIL_SQL_REGISTER_HPP

#include <brig/database/command.hpp>
#include <brig/database/detail/dialect.hpp>
#include <brig/database/detail/get_table_def.hpp>
#include <brig/database/detail/simple_rasters.hpp>
#include <brig/database/detail/sql_create.hpp>
#include <brig/pyramid_def.hpp>
#include <brig/string_cast.hpp>
#include <brig/table_def.hpp>
#include <ios>
#include <iterator>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

inline void sql_register(dialect* dct, command* cmd, const pyramid_def& raster, std::vector<std::string>& sql)
{
  using namespace std;

  table_def tbl;
  cmd->exec("SELECT t.scm, t.tbl FROM (" + dct->sql_tables() + ") t WHERE LOWER(t.tbl) = 'simple_rasters'");
  vector<variant> row;
  if (cmd->fetch(row))
  {
    tbl.id.schema = string_cast<char>(row[0]);
    tbl.id.name = string_cast<char>(row[1]);
    if (cmd->fetch(row)) throw runtime_error("ambiguous simple_rasters error");
    tbl = get_table_def(dct, cmd, tbl.id);
  }
  else
  {
    const string& schema(raster.id.schema);
    tbl = dct->fit_table(simple_rasters_table(!schema.empty()), schema);
    vector<string> strs;
    sql_create(dct, tbl, strs);
    for (auto str(begin(strs)); str != end(strs); ++str)
      cmd->exec(*str);
  }
  auto cols(simple_rasters_columns(tbl));

  for (auto lvl(begin(raster.levels)); lvl != end(raster.levels); ++lvl)
  {
    ostringstream stream; stream.imbue(locale::classic()); stream << scientific; stream.precision(17);
    stream << "INSERT INTO " << dct->sql_identifier(tbl.id) << "(";
    bool first(true);
    for (auto col(begin(cols)); col != end(cols); ++col)
    {
      if (col->empty()) continue;
      if (first) first = false;
      else stream << ", ";
      stream << dct->sql_identifier(*col);
    }
    stream << ") VALUES (";
    if (!lvl->geometry.schema.empty()) stream << "'" << lvl->geometry.schema << "', ";
    stream << "'" << lvl->geometry.name << "', '" << lvl->raster.name << "', ";
    if (!raster.id.schema.empty()) stream << "'" << raster.id.schema << "', ";
    stream << "'" << raster.id.name << "', '" << raster.id.qualifier << "', '" << lvl->geometry.qualifier << "', " << lvl->resolution_x << ", " << lvl->resolution_y << ")";
    sql.push_back(stream.str());
  }
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_REGISTER_HPP
