// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_TABLE_DEFINITION_HPP
#define BRIG_DATABASE_DETAIL_GET_TABLE_DEFINITION_HPP

#include <brig/database/command.hpp>
#include <brig/database/detail/dialect.hpp>
#include <brig/database/detail/get_table_definition_sqlite.hpp>
#include <brig/database/detail/to_lcase.hpp>
#include <brig/database/global.hpp>
#include <brig/database/numeric_cast.hpp>
#include <brig/database/table_definition.hpp>
#include <brig/string_cast.hpp>
#include <iterator>
#include <stdexcept>

namespace brig { namespace database { namespace detail {

inline table_definition get_table_definition(dialect* dct, command* cmd, const identifier& tbl)
{
  using namespace std;

  if (cmd->system() == SQLite) return get_table_definition_sqlite(dct, cmd, tbl);

  // columns
  table_definition res;
  res.id = tbl;
  cmd->exec(dct->sql_columns(res.id));
  vector<variant> row;
  while (cmd->fetch(row))
  {
    column_definition col;
    col.name = string_cast<char>(row[0]);
    col.dbms_type_lcase.schema = to_lcase(string_cast<char>(row[1]));
    col.dbms_type_lcase.name = to_lcase(string_cast<char>(row[2]));
    numeric_cast(row[3], col.chars);
    int scale(-1);
    numeric_cast(row[4], scale);
    col.type = dct->get_type(col.dbms_type_lcase, scale);
    int not_null(0);
    col.not_null = (numeric_cast(row[5], not_null) && not_null);
    res.columns.push_back(col);
  }
  if (res.columns.empty()) throw runtime_error("table error");

  // indexes
  cmd->exec(dct->sql_indexed_columns(res.id));
  index_definition idx;
  while (cmd->fetch(row))
  {
    identifier id;
    id.schema = string_cast<char>(row[0]);
    id.name = string_cast<char>(row[1]);

    if (id.schema != idx.id.schema || id.name != idx.id.name)
    {
      if (VoidIndex != idx.type) res.indexes.push_back(move(idx));

      idx = index_definition();
      idx.id = id;
      int primary(0), unique(0), spatial(0);
      numeric_cast(row[2], primary);
      numeric_cast(row[3], unique);
      numeric_cast(row[4], spatial);
      if (primary) idx.type = Primary;
      else if (unique) idx.type = Unique;
      else if (spatial) idx.type = Spatial;
      else idx.type = Duplicate;
    }

    const string col_name(string_cast<char>(row[5]));
    idx.columns.push_back(col_name);
    if (!find_column(begin(res.columns), end(res.columns), col_name)) idx.type = VoidIndex; // expression

    int desc(0);
    if (numeric_cast(row[6], desc) && desc) idx.type = VoidIndex; // descending
  }
  if (VoidIndex != idx.type) res.indexes.push_back(move(idx));

  // srid, epsg, type qualifier
  for (auto col(begin(res.columns)); col != end(res.columns); ++col)
    if (Geometry == col->type)
    {
      const string sql(dct->sql_spatial_detail(res, col->name));
      cmd->exec(sql);
      if (cmd->fetch(row))
      {
        numeric_cast(row[0], col->srid);
        if (row.size() > 1) numeric_cast(row[1], col->epsg);
        else col->epsg = col->srid;
        if (row.size() > 2) col->dbms_type_lcase.qualifier = to_lcase(string_cast<char>(row[2]));
      }
    }
  return res;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_TABLE_DEFINITION_HPP
