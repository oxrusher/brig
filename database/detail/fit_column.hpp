// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_FIT_COLUMN_HPP
#define BRIG_DATABASE_DETAIL_FIT_COLUMN_HPP

#include <brig/database/column_definition.hpp>
#include <brig/database/detail/fit_identifier.hpp>
#include <brig/database/global.hpp>
#include <brig/string_cast.hpp>
#include <brig/unicode/lower_case.hpp>
#include <brig/unicode/transform.hpp>
#include <stdexcept>

namespace brig { namespace database { namespace detail {

inline column_definition fit_column(const column_definition& col_from, DBMS sys_to)
{
  using namespace std;
  using namespace brig::unicode;

  column_definition col_to;
  col_to.name = fit_identifier(col_from.name, sys_to);
  col_to.type = col_from.type;
  switch (col_to.type)
  {
  default: break;
  case Geometry: col_to.epsg = col_from.epsg; break;
  case String: col_to.chars = (col_from.chars > 0 && col_from.chars < CharsLimit)? col_from.chars: CharsLimit; break;
  }
  col_to.not_null = col_from.not_null;

  switch (sys_to)
  {
  case VoidSystem: throw runtime_error("DBMS error");

  case CUBRID:
    switch (col_to.type)
    {
    case VoidColumn:
    case Geometry: throw runtime_error("datatype error");
    case Blob: col_to.dbms_type.name = "BIT VARYING"; break;
    case Double: col_to.dbms_type.name = "DOUBLE"; break;
    case Integer: col_to.dbms_type.name = "BIGINT"; break;
    case String: col_to.dbms_type.name = "STRING"; break;
    }
    break;

  case DB2:
    switch (col_to.type)
    {
    case VoidColumn: throw runtime_error("datatype error");
    case Blob: col_to.dbms_type.name = "BLOB"; break;
    case Double: col_to.dbms_type.name = "DOUBLE"; break;
    case Geometry:
      col_to.dbms_type.schema = "DB2GSE";
      col_to.dbms_type.name = "ST_GEOMETRY";
      break;
    case Integer: col_to.dbms_type.name = "BIGINT"; break;
    case String: col_to.dbms_type.name = "VARGRAPHIC(" + string_cast<char>(col_to.chars) + ")"; break;
    }
    break;

  case Informix:
    switch (col_to.type)
    {
    case VoidColumn: throw runtime_error("datatype error");
    case Blob: col_to.dbms_type.name = "BYTE"; break;
    case Double: col_to.dbms_type.name = "DOUBLE PRECISION"; break;
    case Geometry: col_to.dbms_type.name = "ST_GEOMETRY"; break;
    case Integer: col_to.dbms_type.name = "INT8"; break;
    case String: col_to.dbms_type.name = "VARCHAR(" + string_cast<char>(col_to.chars) + ")"; break;
    }
    break;

  case Ingres:
    switch (col_to.type)
    {
    case VoidColumn: throw runtime_error("datatype error");
    case Blob: col_to.dbms_type.name = "BLOB"; break;
    case Double: col_to.dbms_type.name = "DOUBLE PRECISION"; break;
    case Geometry: col_to.dbms_type.name = "GEOMETRY"; break;
    case Integer: col_to.dbms_type.name = "BIGINT"; break;
    case String: col_to.dbms_type.name = "NVARCHAR(" + string_cast<char>(col_to.chars) + ")"; break;
    }
    break;

  case MS_SQL:
    switch (col_to.type)
    {
    case VoidColumn: throw runtime_error("datatype error");
    case Blob: col_to.dbms_type.name = "VARBINARY(MAX)"; break;
    case Double: col_to.dbms_type.name = "FLOAT"; break;
    case Geometry: col_to.dbms_type.name = "GEOMETRY"; break;
    case Integer: col_to.dbms_type.name = "BIGINT"; break;
    case String: col_to.dbms_type.name = "NVARCHAR(" + string_cast<char>(col_to.chars) + ")"; break;
    }
    break;

  case MySQL:
    switch (col_to.type)
    {
    case VoidColumn: throw runtime_error("datatype error");
    case Blob: col_to.dbms_type.name = "LONGBLOB"; break;
    case Double: col_to.dbms_type.name = "DOUBLE"; break;
    case Geometry: col_to.dbms_type.name = "GEOMETRY"; break;
    case Integer: col_to.dbms_type.name = "BIGINT"; break;
    case String: col_to.dbms_type.name = "NVARCHAR(" + string_cast<char>(col_to.chars) + ")"; break;
    }
    break;

  case Oracle:
    switch (col_to.type)
    {
    case VoidColumn: throw runtime_error("datatype error");
    case Blob: col_to.dbms_type.name = "BLOB"; break;
    case Double: col_to.dbms_type.name = "BINARY_DOUBLE"; break;
    case Geometry:
      col_to.dbms_type.schema = "MDSYS";
      col_to.dbms_type.name = "SDO_GEOMETRY";
      col_to.query_value = (typeid(blob_t) == col_from.query_value.type())? col_from.query_value: blob_t();
      break;
    case Integer: col_to.dbms_type.name = "NUMBER(19)"; break;
    case String: col_to.dbms_type.name = "NVARCHAR2(" + string_cast<char>(col_to.chars) + ")"; break;
    }
    break;

  case Postgres:
    switch (col_to.type)
    {
    case VoidColumn: throw runtime_error("datatype error");
    case Blob: col_to.dbms_type.name = "BYTEA"; break;
    case Double: col_to.dbms_type.name = "DOUBLE PRECISION"; break;
    case Geometry:
      col_to.dbms_type.schema = "USER-DEFINED";
      col_to.dbms_type.name = "GEOMETRY";
      break;
    case Integer: col_to.dbms_type.name = "BIGINT"; break;
    case String: col_to.dbms_type.name = "VARCHAR(" + string_cast<char>(col_to.chars) + ")"; break;
    }
    break;

  case SQLite:
    switch (col_to.type)
    {
    case VoidColumn: throw runtime_error("datatype error");
    case Blob: col_to.dbms_type.name = "BLOB"; break;
    case Double: col_to.dbms_type.name = "REAL"; break; // real affinity
    case Geometry: col_to.dbms_type.name = "GEOMETRY"; break;
    case Integer: col_to.dbms_type.name = "INTEGER"; break; // integer affinity
    case String: col_to.dbms_type.name = "TEXT"; break; // text affinity
    }
    break;
  }

  col_to.dbms_type_lcase.schema = transform<string>(col_to.dbms_type.schema, lower_case);
  col_to.dbms_type_lcase.name = transform<string>(col_to.dbms_type.name, lower_case);

  return col_to;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_FIT_COLUMN_HPP
