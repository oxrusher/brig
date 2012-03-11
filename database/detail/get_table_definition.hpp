// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_TABLE_DEFINITION_HPP
#define BRIG_DATABASE_DETAIL_GET_TABLE_DEFINITION_HPP

#include <algorithm>
#include <brig/database/column_definition.hpp>
#include <brig/database/command.hpp>
#include <brig/database/detail/get_table_definition_sqlite.hpp>
#include <brig/database/detail/get_type.hpp>
#include <brig/database/detail/sql_columns.hpp>
#include <brig/database/detail/sql_indexed_columns.hpp>
#include <brig/database/detail/sql_srid.hpp>
#include <brig/database/global.hpp>
#include <brig/database/identifier.hpp>
#include <brig/database/index_definition.hpp>
#include <brig/database/numeric_cast.hpp>
#include <brig/database/table_definition.hpp>
#include <brig/database/variant.hpp>
#include <brig/detail/string_cast.hpp>
#include <brig/unicode/lower_case.hpp>
#include <brig/unicode/transform.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

inline table_definition get_table_definition(std::shared_ptr<command> cmd, const identifier& tbl)
{
  using namespace brig::database::detail;
  using namespace brig::detail;
  using namespace brig::unicode;

  const DBMS sys(cmd->system());
  if (SQLite == sys) return get_table_definition_sqlite(cmd, tbl);

  // columns
  table_definition res;
  res.id = tbl;
  cmd->exec(sql_columns(sys, tbl));
  std::vector<variant> row;
  while (cmd->fetch(row))
  {
    column_definition col;
    col.name = string_cast<char>(row[0]);
    col.dbms_type.schema = string_cast<char>(row[1]);
    col.dbms_type.name = string_cast<char>(row[2]); 
    col.lower_case_type.schema = transform<std::string>(col.dbms_type.schema, lower_case);
    col.lower_case_type.name = transform<std::string>(col.dbms_type.name, lower_case);
    col.type = get_type(sys, col);
    numeric_cast(row[3], col.chars);
    numeric_cast(row[4], col.precision);
    numeric_cast(row[5], col.scale);
    res.columns.push_back(col);
  }
  if (res.columns.empty()) throw std::runtime_error("table error");

  // indexes
  cmd->exec(sql_indexed_columns(sys, tbl));
  index_definition idx;
  while (cmd->fetch(row))
  {
    identifier id;
    id.schema = string_cast<char>(row[0]);
    id.name = string_cast<char>(row[1]);

    if (id.schema != idx.id.schema || id.name != idx.id.name)
    {
      if (VoidIndex != idx.type) res.indexes.push_back(std::move(idx));

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

    const std::string col_name(string_cast<char>(row[5]));
    idx.columns.push_back(col_name);
    if (std::find_if(res.columns.begin(), res.columns.end(), [&](const column_definition& col){ return col.name == col_name; }) == res.columns.end()) idx.type = VoidIndex; // expression

    int desc(0);
    if (numeric_cast(row[6], desc) && desc) idx.type = VoidIndex; // descending
  }
  if (VoidIndex != idx.type) res.indexes.push_back(std::move(idx));

  // srid, epsg, type qualifier
  for (auto p_col = res.columns.begin(); p_col != res.columns.end(); ++p_col)
  {
    const std::string sql(sql_srid(sys, tbl, *p_col));
    if (!sql.empty())
    {
      cmd->exec(sql);
      if (cmd->fetch(row))
      {
        numeric_cast(row[0], p_col->srid);
        if (row.size() > 1) numeric_cast(row[1], p_col->epsg);
        else p_col->epsg = p_col->srid;
        if (row.size() > 2)
        {
          p_col->dbms_type.qualifier = string_cast<char>(row[2]);
          p_col->lower_case_type.qualifier = transform<std::string>(p_col->dbms_type.qualifier, lower_case);
        }
      }
    }
  }
  return res;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_TABLE_DEFINITION_HPP