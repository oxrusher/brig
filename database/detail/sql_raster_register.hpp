// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_RASTER_REGISTER_HPP
#define BRIG_DATABASE_DETAIL_SQL_RASTER_REGISTER_HPP

#include <brig/database/command.hpp>
#include <brig/database/detail/get_schema.hpp>
#include <brig/database/detail/get_table_definition.hpp>
#include <brig/database/detail/simple_rasters.hpp>
#include <brig/database/detail/sql_create.hpp>
#include <brig/database/detail/sql_identifier.hpp>
#include <brig/database/detail/sql_tables.hpp>
#include <brig/database/global.hpp>
#include <brig/database/raster_pyramid.hpp>
#include <brig/database/table_definition.hpp>
#include <brig/database/variant.hpp>
#include <brig/string_cast.hpp>
#include <ios>
#include <iterator>
#include <locale>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

inline void sql_raster_register(std::shared_ptr<command> cmd, const raster_pyramid& raster, std::vector<std::string>& sql)
{
  using namespace std;

  const DBMS sys(cmd->system());
  const string schema(get_schema(cmd));
  table_definition tbl;
  cmd->exec(sql_tables(sys, "simple_rasters", false));
  vector<variant> row;
  if (cmd->fetch(row))
  {
    tbl.id.schema = string_cast<char>(row[0]);
    tbl.id.name = string_cast<char>(row[1]);
    if (cmd->fetch(row)) throw runtime_error("ambiguous simple_rasters error");
    tbl = get_table_definition(cmd, tbl.id);
  }
  else
  {
    tbl = simple_rasters_table(sys, schema);
    vector<string> strs;
    sql_create(sys, tbl, strs);
    for (auto str(begin(strs)); str != end(strs); ++str)
      cmd->exec(*str);
  }
  auto cols = simple_rasters_columns(tbl);

  for (auto lvl(begin(raster.levels)); lvl != end(raster.levels); ++lvl)
  {
    ostringstream reg; reg.imbue(locale::classic()); reg << scientific; reg.precision(17);
    reg << "INSERT INTO " << sql_identifier(sys, tbl.id) << "(";
    bool first(true);
    for (auto col(begin(cols)); col != end(cols); ++col)
    {
      if (col->empty()) continue;
      if (first) first = false;
      else reg << ", ";
      reg << sql_identifier(sys, *col);
    }
    reg << ") VALUES (";
    if (!lvl->geometry.schema.empty()) reg << "'" << lvl->geometry.schema << "', ";
    reg << "'" << lvl->geometry.name << "', '" << lvl->raster.name << "', ";
    if (!raster.id.schema.empty()) reg << "'" << raster.id.schema << "', ";
    reg << "'" << raster.id.name << "', '" << raster.id.qualifier << "', '" << lvl->geometry.qualifier << "', " << lvl->resolution.get<0>() << ", " << lvl->resolution.get<1>() << ")";
    sql.push_back(reg.str());
  }
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_RASTER_REGISTER_HPP
