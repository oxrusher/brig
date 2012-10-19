// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SIMPLE_RASTERS_HPP
#define BRIG_DATABASE_DETAIL_SIMPLE_RASTERS_HPP

#include <brig/database/detail/fit_table.hpp>
#include <brig/database/global.hpp>
#include <brig/database/table_definition.hpp>
#include <brig/unicode/lower_case.hpp>
#include <brig/unicode/transform.hpp>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

inline column_definition simple_rasters_column(DBMS sys, const std::string& name, column_type type)
{
  column_definition col;
  col.name = name;
  col.type = type;
  col.not_null = true;
  if (MySQL == sys && String == type)
    // Max key length is 1000 bytes.
    // The problem with UTF-8 is that every character reserves 3 bytes for the worst case.
    // The key uses 3 * 100 = 300 characters == 900 bytes
    col.chars = 100;
  return col;
}

inline table_definition simple_rasters_table(DBMS sys, const std::string& schema)
{
  table_definition tbl;
  tbl.id.name = "simple_rasters";

  tbl.indexes.push_back(index_definition());
  index_definition& idx(tbl.indexes.back());
  idx.type = Primary;

  if (SQLite != sys)  { tbl.columns.push_back( simple_rasters_column(sys, "schema", String) ); idx.columns.push_back("schema"); }
  tbl.columns.push_back( simple_rasters_column(sys, "table", String) ); idx.columns.push_back("table");
  tbl.columns.push_back( simple_rasters_column(sys, "raster", String) ); idx.columns.push_back("raster");

  if (SQLite != sys) tbl.columns.push_back( simple_rasters_column(sys, "base_schema", String) );
  tbl.columns.push_back( simple_rasters_column(sys, "base_table", String) );
  tbl.columns.push_back( simple_rasters_column(sys, "base_raster", String) );

  tbl.columns.push_back( simple_rasters_column(sys, "geometry", String) );
  tbl.columns.push_back( simple_rasters_column(sys, "resolution_x", Double) );
  tbl.columns.push_back( simple_rasters_column(sys, "resolution_y", Double) );

  return fit_table(tbl, sys, schema);
}

inline std::vector<std::string> simple_rasters_columns(const table_definition& tbl)
{
  using namespace std;
  using namespace brig::unicode;

  vector<string> cols;
  cols.resize(9);
  for (size_t i(0); i < tbl.columns.size(); ++i)
  {
    const string& name(tbl.columns[i].name);
    const string name_lcase(transform<string>(name, lower_case));

    if (name_lcase.compare("schema") == 0) cols[0] = name;
    else if (name_lcase.compare("table") == 0) cols[1] = name;
    else if (name_lcase.compare("raster") == 0) cols[2] = name;
    else if (name_lcase.compare("base_schema") == 0) cols[3] = name;
    else if (name_lcase.compare("base_table") == 0) cols[4] = name;
    else if (name_lcase.compare("base_raster") == 0) cols[5] = name;
    else if (name_lcase.compare("geometry") == 0) cols[6] = name;
    else if (name_lcase.compare("resolution_x") == 0) cols[7] = name;
    else if (name_lcase.compare("resolution_y") == 0) cols[8] = name;
  }
  return cols;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SIMPLE_RASTERS_HPP
