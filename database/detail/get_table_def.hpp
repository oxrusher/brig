// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_TABLE_DEF_HPP
#define BRIG_DATABASE_DETAIL_GET_TABLE_DEF_HPP

#include <brig/database/command.hpp>
#include <brig/database/detail/dialect.hpp>
#include <brig/database/detail/get_table_def_sqlite.hpp>
#include <brig/global.hpp>
#include <brig/numeric_cast.hpp>
#include <brig/string_cast.hpp>
#include <brig/table_def.hpp>
#include <brig/unicode/lower_case.hpp>
#include <brig/unicode/transform.hpp>
#include <iterator>
#include <stdexcept>

namespace brig { namespace database { namespace detail {

inline table_def get_table_def(dialect* dct, command* cmd, const identifier& tbl)
{
  using namespace std;
  using namespace brig::unicode;

  if (cmd->system() == DBMS::SQLite) return get_table_def_sqlite(dct, cmd, tbl);

  // columns
  table_def res;
  res.id = tbl;
  cmd->exec(dct->sql_columns(res.id));
  vector<variant> row;
  while (cmd->fetch(row))
  {
    column_def col;
    col.name = string_cast<char>(row[0]);
    col.type_lcase.schema = transform<char>(string_cast<char>(row[1]), lower_case);
    col.type_lcase.name = transform<char>(string_cast<char>(row[2]), lower_case);
    numeric_cast(row[3], col.chars);
    int scale(-1);
    numeric_cast(row[4], scale);
    col.type = dct->get_type(col.type_lcase, scale);
    int not_null(0);
    col.not_null = (numeric_cast(row[5], not_null) && not_null);
    res.columns.push_back(col);
  }
  if (res.columns.empty()) throw runtime_error("table error");

  // indexes
  cmd->exec(dct->sql_indexed_columns(res.id));
  index_def idx;
  while (cmd->fetch(row))
  {
    identifier id = { string_cast<char>(row[0]), string_cast<char>(row[1]), "" };

    if (id.schema != idx.id.schema || id.name != idx.id.name)
    {
      if (index_type::Void != idx.type) res.indexes.push_back(move(idx));

      idx = index_def();
      idx.id = id;
      int primary(0), unique(0), spatial(0);
      numeric_cast(row[2], primary);
      numeric_cast(row[3], unique);
      numeric_cast(row[4], spatial);
      if (primary) idx.type = index_type::Primary;
      else if (unique) idx.type = index_type::Unique;
      else if (spatial) idx.type = index_type::Spatial;
      else idx.type = index_type::Duplicate;
    }

    const string col_name(string_cast<char>(row[5]));
    idx.columns.push_back(col_name);
    if (!find_column(begin(res.columns), end(res.columns), col_name)) idx.type = index_type::Void; // expression

    int desc(0);
    if (numeric_cast(row[6], desc) && desc) idx.type = index_type::Void; // descending
  }
  if (index_type::Void != idx.type) res.indexes.push_back(move(idx));

  // srid, epsg, type qualifier
  for (auto& col: res.columns)
    if (column_type::Geometry == col.type)
    {
      const string sql(dct->sql_spatial_detail(res, col.name));
      cmd->exec(sql);
      if (cmd->fetch(row))
      {
        numeric_cast(row[0], col.srid);
        if (row.size() > 1) numeric_cast(row[1], col.epsg);
        else col.epsg = col.srid;
        if (row.size() > 2) col.type_lcase.qualifier = transform<char>(string_cast<char>(row[2]), lower_case);
      }
    }
  return res;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_TABLE_DEF_HPP
