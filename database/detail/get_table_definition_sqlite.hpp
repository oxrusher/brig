// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_TABLE_DEFINITION_SQLITE_HPP
#define BRIG_DATABASE_DETAIL_GET_TABLE_DEFINITION_SQLITE_HPP

#include <algorithm>
#include <brig/database/command.hpp>
#include <brig/database/detail/dialect.hpp>
#include <brig/database/detail/get_iso_type.hpp>
#include <brig/database/detail/is_ogc_type.hpp>
#include <brig/database/detail/to_lcase.hpp>
#include <brig/database/global.hpp>
#include <brig/database/numeric_cast.hpp>
#include <brig/string_cast.hpp>
#include <iterator>
#include <stdexcept>

namespace brig { namespace database { namespace detail {

inline table_definition get_table_definition_sqlite(dialect* dct, command* cmd, const identifier& tbl)
{
  using namespace std;

  // columns
  table_definition res;
  res.id = tbl;
  vector<string> keys;
  vector<variant> row;
  cmd->exec("PRAGMA TABLE_INFO(" + dct->sql_identifier(tbl.name) + ")");
  while (cmd->fetch(row))
  {
    column_definition col;
    col.name = string_cast<char>(row[1]);
    col.dbms_type_lcase.name = to_lcase(string_cast<char>(row[2]));

    if (is_ogc_type(col.dbms_type_lcase.name)) col.type = Geometry;
    else col.type = get_iso_type(col.dbms_type_lcase.name, -1);

    int not_null(0);
    col.not_null = (numeric_cast(row[3], not_null) && not_null);
    res.columns.push_back(col);

    int key(0);
    if (numeric_cast(row[5], key) && key) keys.push_back(col.name);
  }
  if (res.columns.empty()) throw runtime_error("table error");

  if (!keys.empty())
  {
    index_definition pri_idx;
    pri_idx.type = Primary;
    pri_idx.columns = keys;
    res.indexes.push_back(pri_idx);
    sort(begin(keys), end(keys));
  }

  // indexes
  cmd->exec("PRAGMA INDEX_LIST(" + dct->sql_identifier(tbl.name) + ")");
  while (cmd->fetch(row))
  {
    index_definition idx;
    idx.id.name = string_cast<char>(row[1]);
    int unique(0);
    idx.type = (numeric_cast(row[2], unique) && !unique)? Duplicate: Unique;
    res.indexes.push_back(idx);
  }

  // indexed columns
  for (size_t i(0); i < res.indexes.size(); ++i)
  {
    if (res.indexes[i].id.name.empty()) continue;

    cmd->exec("PRAGMA INDEX_INFO(" + dct->sql_identifier(res.indexes[i].id.name) + ")");
    vector<string> cols;
    vector<pair<int, string>> seq_cols;
    while (cmd->fetch(row))
    {
      const string col(string_cast<char>(row[2]));
      cols.push_back(col);

      pair<int, string> seq_col;
      numeric_cast(row[0], seq_col.first);
      seq_col.second = col;
      seq_cols.push_back(seq_col);
    }
    sort(begin(cols), end(cols));
    sort(begin(seq_cols), end(seq_cols));
    for (size_t j(0); j < seq_cols.size(); ++j)
      res.indexes[i].columns.push_back(seq_cols[j].second);

    if (keys.size() == cols.size() && equal(begin(keys), end(keys), begin(cols)))
    {
      keys.clear();
      res.indexes[i].type = VoidIndex;
      res.indexes[0].columns = move(res.indexes[i].columns);
    }
  }
  auto new_end(remove_if(begin(res.indexes), end(res.indexes), [](const index_definition& idx){ return VoidIndex == idx.type; }));
  res.indexes.resize(distance(begin(res.indexes), new_end));

  // srid, epsg, spatial index
  for (size_t i(0); i < res.columns.size(); ++i)
  {
    if (Geometry == res.columns[i].type)
    {
      cmd->exec("\
SELECT c.SRID, (CASE s.AUTH_NAME WHEN 'epsg' THEN s.AUTH_SRID ELSE NULL END) epsg, c.SPATIAL_INDEX_ENABLED \
FROM (SELECT SRID, SPATIAL_INDEX_ENABLED FROM GEOMETRY_COLUMNS WHERE F_TABLE_NAME = '" + tbl.name + "' AND F_GEOMETRY_COLUMN = '" + res.columns[i].name + "') c \
LEFT JOIN SPATIAL_REF_SYS s ON c.SRID = s.SRID");
      if (cmd->fetch(row))
      {
        numeric_cast(row[0], res.columns[i].srid);
        numeric_cast(row[1], res.columns[i].epsg);
        int indexed(0);
        if (numeric_cast(row[2], indexed) && indexed == 1)
        {
          index_definition idx;
          idx.type = Spatial;
          idx.columns.push_back(res.columns[i].name);
          res.indexes.push_back(idx);
        }
      }
    }
  }
  return res;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_TABLE_DEFINITION_SQLITE_HPP
