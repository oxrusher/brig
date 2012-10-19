// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_RASTER_UNREGISTER_HPP
#define BRIG_DATABASE_DETAIL_SQL_RASTER_UNREGISTER_HPP

#include <brig/database/command.hpp>
#include <brig/database/detail/get_table_definition.hpp>
#include <brig/database/detail/simple_rasters.hpp>
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

inline void sql_raster_unregister(std::shared_ptr<command> cmd, const raster_pyramid& raster, std::vector<std::string>& sql)
{
  using namespace std;

  const DBMS sys(cmd->system());
  cmd->exec(sql_tables(sys, "simple_rasters", false));
  vector<variant> row;
  if (!cmd->fetch(row)) throw runtime_error("simple_rasters error");
  table_definition tbl;
  tbl.id.schema = string_cast<char>(row[0]);
  tbl.id.name = string_cast<char>(row[1]);
  if (cmd->fetch(row)) throw runtime_error("ambiguous simple_rasters error");
  tbl = get_table_definition(cmd, tbl.id);
  auto cols = simple_rasters_columns(tbl);

  for (auto lvl(begin(raster.levels)); lvl != end(raster.levels); ++lvl)
  {
    string unreg;
    unreg += "DELETE FROM " + sql_identifier(sys, tbl.id) + " WHERE ";
    if (!lvl->geometry.schema.empty()) unreg += sql_identifier(sys, cols[0]) + " = '" + lvl->geometry.schema + "' AND ";
    unreg += sql_identifier(sys, cols[1]) + " = '" + lvl->geometry.name + "' AND " + sql_identifier(sys, cols[2]) + " = '" + lvl->raster.name + "'";
    sql.push_back(unreg);
  }
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_RASTER_UNREGISTER_HPP
