// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SIMPLE_RASTERS_HPP
#define BRIG_DATABASE_DETAIL_SIMPLE_RASTERS_HPP

#include <brig/database/detail/to_lcase.hpp>
#include <brig/database/global.hpp>
#include <brig/database/table_definition.hpp>

namespace brig { namespace database { namespace detail {

inline column_definition simple_rasters_column(const std::string& name, column_type type)
{
  column_definition col;
  col.name = name;
  col.type = type;
  col.not_null = true;
  if (String == type)
    // MySQL:
    // Max key length is 1000 bytes.
    // The problem with UTF-8 is that every character reserves 3 bytes for the worst case.
    // The key uses 3 * 100 = 300 characters == 900 bytes
    col.chars = 100;
  return col;
}

inline table_definition simple_rasters_table(bool schema)
{
  table_definition tbl;
  tbl.id.name = "simple_rasters";

  tbl.indexes.push_back(index_definition());
  index_definition& idx(tbl.indexes.back());
  idx.type = Primary;

  if (schema)  { tbl.columns.push_back( simple_rasters_column("schema", String) ); idx.columns.push_back("schema"); }
  tbl.columns.push_back( simple_rasters_column("table", String) ); idx.columns.push_back("table");
  tbl.columns.push_back( simple_rasters_column("raster", String) ); idx.columns.push_back("raster");

  if (schema) tbl.columns.push_back( simple_rasters_column("base_schema", String) );
  tbl.columns.push_back( simple_rasters_column("base_table", String) );
  tbl.columns.push_back( simple_rasters_column("base_raster", String) );

  tbl.columns.push_back( simple_rasters_column("geometry", String) );
  tbl.columns.push_back( simple_rasters_column("resolution_x", Double) );
  tbl.columns.push_back( simple_rasters_column("resolution_y", Double) );

  return tbl;
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
    const string name_lcase(to_lcase(name));

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
