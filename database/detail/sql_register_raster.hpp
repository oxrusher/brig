// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_REGISTER_RASTER_HPP
#define BRIG_DATABASE_DETAIL_SQL_REGISTER_RASTER_HPP

#include <brig/database/column_definition.hpp>
#include <brig/database/command.hpp>
#include <brig/database/detail/get_table_definition.hpp>
#include <brig/database/detail/sql_identifier.hpp>
#include <brig/database/detail/sql_create.hpp>
#include <brig/database/detail/sql_tables.hpp>
#include <brig/database/global.hpp>
#include <brig/database/identifier.hpp>
#include <brig/database/index_definition.hpp>
#include <brig/database/raster_definition.hpp>
#include <brig/database/table_definition.hpp>
#include <brig/database/variant.hpp>
#include <brig/detail/string_cast.hpp>
#include <ios>
#include <locale>
#include <memory>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

inline column_definition get_column_definition(const std::string& name, column_type type)
{
  column_definition col;
  col.name = name;
  col.type = type;
  col.not_null = true;
  return col;
}

inline table_definition get_simple_rasters_definition(DBMS sys)
{
  table_definition tbl;
  tbl.indexes.push_back(index_definition());
  index_definition& idx(tbl.indexes.back());
  idx.type = Primary;

  if (SQLite != sys)  { tbl.columns.push_back( get_column_definition("schema", String) ); idx.columns.push_back("schema"); }
  tbl.columns.push_back( get_column_definition("table", String) ); idx.columns.push_back("table");
  tbl.columns.push_back( get_column_definition("raster", String) ); idx.columns.push_back("raster");

  if (SQLite != sys) tbl.columns.push_back( get_column_definition("base_schema", String) );
  tbl.columns.push_back( get_column_definition("base_table", String) );
  tbl.columns.push_back( get_column_definition("base_raster", String) );

  tbl.columns.push_back( get_column_definition("geometry", String) );
  tbl.columns.push_back( get_column_definition("resolution_x", Double) );
  tbl.columns.push_back( get_column_definition("resolution_y", Double) );

  return tbl;
}

inline std::vector<std::string> sql_register_raster(std::shared_ptr<command> cmd, const raster_definition& raster)
{
  using namespace brig::detail;

  const DBMS sys(cmd->system());
  cmd->exec(sql_tables(sys, "simple_rasters"));
  std::vector<variant> row;
  if (!cmd->fetch(row))
  {
    auto sqls = sql_create(sys, get_simple_rasters_definition(sys));
    for (auto sql = sqls.begin(); sql != sqls.end(); ++sql)
      cmd->exec(*sql);

    cmd->exec(sql_tables(sys, "simple_rasters"));
    if (!cmd->fetch(row)) throw std::runtime_error("sql error");
  }

  identifier id;
  id.schema = string_cast<char>(row[0]);
  id.name = string_cast<char>(row[1]);
  auto tbl = get_table_definition(cmd, id);
  std::vector<std::string> res;

  for (size_t lvl(0); lvl < raster.levels.size(); ++lvl)
  {
    std::ostringstream stream; stream.imbue(std::locale::classic()); stream << std::scientific; stream.precision(16);
    stream << "INSERT INTO " << sql_identifier(sys, tbl.id) << "(";
    for (size_t col(0); col < tbl.columns.size(); ++col)
    {
      if (col > 0) stream << ", ";
      stream << sql_identifier(sys, tbl.columns[col].name);
    }
    stream << ") VALUES (";
    for (size_t col(0); col < tbl.columns.size(); ++col)
    {
      if (col > 0) stream << ", ";
      if ("schema" == tbl.columns[col].name) stream << "'" << raster.levels[lvl].geometry_layer.schema << "'";
      else if ("table" == tbl.columns[col].name) stream << "'" << raster.levels[lvl].geometry_layer.name << "'";
      else if ("raster" == tbl.columns[col].name)
      {
        stream << "'";
        if (typeid(column_definition) == raster.levels[lvl].raster_column.type())
          stream << ::boost::get<column_definition>(raster.levels[lvl].raster_column).name;
        else
          stream << ::boost::get<std::string>(raster.levels[lvl].raster_column);
        stream << "'";
      }
      else if ("base_schema" == tbl.columns[col].name) stream << "'" << raster.id.schema << "'";
      else if ("base_table" == tbl.columns[col].name) stream << "'" << raster.id.name << "'";
      else if ("base_raster" == tbl.columns[col].name) stream << "'" << raster.id.qualifier << "'";
      else if ("geometry" == tbl.columns[col].name) stream << "'" << raster.levels[lvl].geometry_layer.qualifier << "'";
      else if ("resolution_x" == tbl.columns[col].name) stream << raster.levels[lvl].resolution.get<0>();
      else if ("resolution_y" == tbl.columns[col].name) stream << raster.levels[lvl].resolution.get<1>();
    }
    stream << ")";
    res.push_back(stream.str());
  }

  return res;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_REGISTER_RASTER_HPP
