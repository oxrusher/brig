// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_TYPE_IDENTIFIER_HPP
#define BRIG_DATABASE_DETAIL_GET_TYPE_IDENTIFIER_HPP

#include <boost/algorithm/string.hpp>
#include <brig/database/column_detail.hpp>
#include <brig/database/dbms.hpp>
#include <brig/database/detail/is_geometry_type.hpp>
#include <brig/database/type_identifier.hpp>

namespace brig { namespace database { namespace detail {

inline TypeIdentifier get_type_identifier(DBMS sys, const column_detail& col)
{
  if (is_geometry_type(sys, col))
    return Geometry;
  else
    switch (sys)
    {
    case UnknownSystem:
    case SQLite:
      break;

    case DB2:
      if (!col.type_schema.empty() && col.type_schema != "SYSIBM")
        return UnknownType;
      else if (boost::algorithm::contains(col.type_name, "GRAPHIC"))
        return String;
      break;

    case MS_SQL:
      if (col.type_name == "BIT")
        return Integer;
      else if (col.type_name == "DATETIME"
            || col.type_name == "DATETIME2"
            || col.type_name == "SMALLDATETIME")
        return DateTime;
      break;

    case MySQL:
      if (col.type_name == "FIXED")
        return col.scale == 0? Integer: Double;
      else if (col.type_name == "DATETIME")
        return DateTime;
      break;

    case Oracle:
      if (!col.type_schema.empty())
        return UnknownType;
      else if (col.type_name == "LONG")
        return String;
      break;

    case Postgres:
      if (col.type_schema == "USER-DEFINED")
        return UnknownType;
      else if (boost::algorithm::contains(col.type_name, "SERIAL"))
        return Integer;
      break;
    }

  if (col.type_name == "DATE")
    return Date;
  else if (boost::algorithm::starts_with(col.type_name, "TIMESTAMP"))
    return DateTime;
  else if (boost::algorithm::contains(col.type_name, "INT")
        || boost::algorithm::starts_with(col.type_name, "BOOL"))
    return Integer;
  else if (boost::algorithm::contains(col.type_name, "CHAR")
        || boost::algorithm::contains(col.type_name, "CLOB")
        || boost::algorithm::contains(col.type_name, "TEXT"))
    return String;
  else if (boost::algorithm::contains(col.type_name, "REAL")
        || boost::algorithm::contains(col.type_name, "FLOAT")
        || boost::algorithm::contains(col.type_name, "DOUBLE"))
    return Double;
  else if (boost::algorithm::starts_with(col.type_name, "DEC")
        || boost::algorithm::starts_with(col.type_name, "NUM"))
    return col.scale == 0? Integer: Double;
  else
    return UnknownType;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_TYPE_IDENTIFIER_HPP
