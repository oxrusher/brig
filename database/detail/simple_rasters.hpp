// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SIMPLE_RASTERS_HPP
#define BRIG_DATABASE_DETAIL_SIMPLE_RASTERS_HPP

#include <brig/table_def.hpp>
#include <brig/unicode/lower_case.hpp>
#include <brig/unicode/transform.hpp>

namespace brig { namespace database { namespace detail {

inline column_def simple_rasters_column(const std::string& name, column_type type)
{
  column_def col;
  col.name = name;
  col.type = type;
  col.not_null = true;
  if (column_type::String == type)
    // MySQL:
    // Max key length is 1000 bytes.
    // The problem with UTF-8 is that every character reserves 3 bytes for the worst case.
    // The key uses 3 * 100 = 300 characters == 900 bytes
    col.chars = 100;
  return col;
}

inline table_def simple_rasters_table(bool schema)
{
  table_def tbl;
  tbl.id.name = "simple_rasters";

  tbl.indexes.push_back(index_def());
  index_def& idx(tbl.indexes.back());
  idx.type = index_type::Primary;

  if (schema)  { tbl.columns.push_back( simple_rasters_column("schema", column_type::String) ); idx.columns.push_back("schema"); }
  tbl.columns.push_back( simple_rasters_column("table", column_type::String) ); idx.columns.push_back("table");
  tbl.columns.push_back( simple_rasters_column("raster", column_type::String) ); idx.columns.push_back("raster");

  if (schema) tbl.columns.push_back( simple_rasters_column("base_schema", column_type::String) );
  tbl.columns.push_back( simple_rasters_column("base_table", column_type::String) );
  tbl.columns.push_back( simple_rasters_column("base_raster", column_type::String) );

  tbl.columns.push_back( simple_rasters_column("geometry", column_type::String) );
  tbl.columns.push_back( simple_rasters_column("resolution_x", column_type::Double) );
  tbl.columns.push_back( simple_rasters_column("resolution_y", column_type::Double) );

  return tbl;
}

inline std::vector<std::string> simple_rasters_columns(const table_def& tbl)
{
  using namespace std;

  vector<string> cols;
  cols.resize(9);
  for (size_t i(0); i < tbl.columns.size(); ++i)
  {
    const string& name(tbl.columns[i].name);
    const string name_lcase(brig::unicode::transform<char>(name, brig::unicode::lower_case));

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
