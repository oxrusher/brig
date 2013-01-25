// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_CREATE_HPP
#define BRIG_DATABASE_DETAIL_SQL_CREATE_HPP

#include <algorithm>
#include <brig/database/detail/dialect.hpp>
#include <brig/global.hpp>
#include <brig/table_def.hpp>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

inline void sql_create(dialect* dct, const table_def& tbl, std::vector<std::string>& sql)
{
  using namespace std;

  {
    string str;
    str += "CREATE TABLE " + dct->sql_identifier(tbl.id.name) + " (";

    bool first(true);
    for (auto col(begin(tbl.columns)); col != end(tbl.columns); ++col)
    {
      const string col_def(dct->sql_column_def(*col));
      if (col_def.empty()) continue;
      if (first) first = false;
      else str += ", ";
      str += col_def;
    }

    auto idx(find_if(begin(tbl.indexes), end(tbl.indexes), [&](const index_def& idx_){ return Primary == idx_.type; }));
    if (idx != end(tbl.indexes))
    {
      str += ", PRIMARY KEY (";
      for (auto col(begin(idx->columns)); col != end(idx->columns); ++col)
      {
        if (col != begin(idx->columns)) str += ", ";
        str += dct->sql_identifier(*col);
      }
      str += ")";
    }

    str += ") " + dct->sql_table_options();
    sql.push_back(str);
  }

  for (auto col(begin(tbl.columns)); col != end(tbl.columns); ++col)
    if (Geometry == col->type)
      dct->sql_register_spatial_column(tbl, col->name, sql);

  for (auto idx(begin(tbl.indexes)); idx != end(tbl.indexes); ++idx)
    switch (idx->type)
    {
    default: throw runtime_error("index error");
    case Primary: break;
    case Unique:
    case Duplicate:
      {
      string str;
      str += "CREATE ";
      if (Unique == idx->type) str += "UNIQUE ";
      str += "INDEX " + dct->sql_identifier(idx->id.name) + " ON " + dct->sql_identifier(tbl.id.name) + " (";
      for (auto col(begin(idx->columns)); col != end(idx->columns); ++col)
      {
        if (col != begin(idx->columns)) str += ", ";
        str += dct->sql_identifier(*col);
      }
      str += ")";
      sql.push_back(str);
      }
      break;
    case Spatial: sql.push_back(dct->sql_create_spatial_index(tbl, idx->columns.front())); break;
    }
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_CREATE_HPP
