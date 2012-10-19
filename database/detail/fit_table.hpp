// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_FIT_TABLE_HPP
#define BRIG_DATABASE_DETAIL_FIT_TABLE_HPP

#include <algorithm>
#include <brig/database/detail/fit_column.hpp>
#include <brig/database/detail/fit_identifier.hpp>
#include <brig/database/global.hpp>
#include <brig/database/table_definition.hpp>
#include <brig/string_cast.hpp>
#include <brig/unicode/lower_case.hpp>
#include <brig/unicode/transform.hpp>
#include <iterator>
#include <stdexcept>

namespace brig { namespace database { namespace detail {

inline table_definition fit_table(const table_definition& tbl_from, DBMS sys_to, const std::string& schema_to)
{
  using namespace std;
  using namespace brig::unicode;

  table_definition tbl_to;
  tbl_to.id = fit_identifier(tbl_from.id, sys_to, schema_to);

  for (auto col_from(begin(tbl_from.columns)); col_from != end(tbl_from.columns); ++col_from)
    tbl_to.columns.push_back(fit_column(*col_from, sys_to));

  auto indexes(tbl_from.indexes);
  switch (sys_to)
  {
  default:
    break;

  case MS_SQL:
    if (find_if(begin(indexes), end(indexes), [&](const index_definition& idx){ return Primary == idx.type; }) == end(indexes))
    {
      auto unq_idx(find_if(begin(indexes), end(indexes), [&](const index_definition& idx){ return Unique == idx.type; }));
      if (unq_idx == end(indexes))
      {
        column_definition col;
        col.name = fit_identifier("ID", sys_to);
        col.type = Integer;
        col.dbms_type.name = "INT IDENTITY";
        col.dbms_type_lcase.name = transform<string>(col.dbms_type.name, lower_case);
        tbl_to.columns.push_back(col);

        index_definition idx;
        idx.type = Primary;
        idx.columns.push_back("ID");
        indexes.push_back(idx);
      }
      else
        unq_idx->type = Primary;
    }
    break;

  case Oracle:
    if (find_if(begin(indexes), end(indexes), [&](const index_definition& idx){ return Primary == idx.type || Unique == idx.type; }) == end(indexes))
    {
      column_definition col;
      col.name = fit_identifier("ID", sys_to);
      col.type = String;
      col.dbms_type.name = "NVARCHAR2(32) DEFAULT SYS_GUID()";
      col.dbms_type_lcase.name = transform<string>(col.dbms_type.name, lower_case);
      col.chars = 32;
      tbl_to.columns.push_back(col);

      index_definition idx;
      idx.type = Primary;
      idx.columns.push_back("ID");
      indexes.push_back(idx);
    }
    break;
  }

  for (auto col_to(begin(tbl_to.columns)); col_to != end(tbl_to.columns); ++col_to)
    if (Geometry == col_to->type && !find_rtree(begin(indexes), end(indexes), col_to->name))
    {
      index_definition idx;
      idx.type = Spatial;
      idx.columns.push_back(col_to->name);
      indexes.push_back(idx);
    }

  size_t suffix(0);
  for (auto idx(begin(indexes)); idx != end(indexes); ++idx)
  {
    index_definition idx_to;
    idx_to.type = idx->type;

    switch (sys_to)
    {
    case VoidSystem:
      throw runtime_error("DBMS error");
    case CUBRID:
    case MS_SQL:
    case MySQL:
      if (Primary != idx_to.type)
        idx_to.id.name = fit_identifier(tbl_to.id.name + "_idx_" + string_cast<char>(++suffix), sys_to);
      break;
    case DB2:
    case Informix:
    case Oracle:
    case Postgres:
      if (Primary != idx_to.type)
      {
        idx_to.id.schema = schema_to;
        idx_to.id.name = fit_identifier(tbl_to.id.name + "_idx_" + string_cast<char>(++suffix), sys_to);
      }
      break;
    case Ingres:
      if (Spatial == idx_to.type)
        continue; // todo:
      else if (Primary != idx_to.type)
      {
        idx_to.id.schema = schema_to;
        idx_to.id.name = fit_identifier(tbl_to.id.name + "_idx_" + string_cast<char>(++suffix), sys_to);
      }
      break;
    case SQLite:
      if (Primary != idx_to.type && Spatial != idx_to.type)
        idx_to.id.name = fit_identifier(tbl_to.id.name + "_idx_" + string_cast<char>(++suffix), sys_to);
      break;
    }

    for (auto col_from(begin(idx->columns)); col_from != end(idx->columns); ++col_from)
    {
      const string col_to(fit_identifier(*col_from, sys_to));
      idx_to.columns.push_back(col_to);
      switch (sys_to)
      {
      case VoidSystem:
        throw runtime_error("DBMS error");
      case CUBRID:
      case Oracle:
      case Postgres:
      case SQLite:
        break;
      case DB2:
        // When UNIQUE is used, null values are treated as any other values. For example, if the key is a single column that may contain null values, that column may contain no more than one null value.
        if (Primary == idx_to.type || Unique == idx_to.type)
          tbl_to[col_to]->not_null = true;
        break;
      case Informix:
        // Null values are never allowed in a primary-key column
        if (Primary == idx_to.type)
          tbl_to[col_to]->not_null = true;
        break;
      case Ingres:
        // All columns in a UNIQUE constraint MUST be created as NOT NULL
        if (Primary == idx_to.type || Unique == idx_to.type)
          tbl_to[col_to]->not_null = true;
        // WITH STRUCTURE=RTREE, RANGE=
        if (Spatial == idx_to.type)
          tbl_to[col_to]->query_value = (typeid(blob_t) == tbl_from[*col_from]->query_value.type())? tbl_from[*col_from]->query_value: blob_t();
        break;
      case MS_SQL:
        // USING GEOMETRY_GRID WITH (BOUNDING_BOX=
        if (Spatial == idx_to.type)
          tbl_to[col_to]->query_value = (typeid(blob_t) == tbl_from[*col_from]->query_value.type())? tbl_from[*col_from]->query_value: blob_t();
        break;
      case MySQL:
        // columns in spatial indexes must be declared NOT NULL
        if (Spatial == idx_to.type)
          tbl_to[col_to]->not_null = true;
        break;
      }
    }

    tbl_to.indexes.push_back(idx_to);
  }

  return tbl_to;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_FIT_TABLE_HPP
