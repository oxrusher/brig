// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_TYPE_HPP
#define BRIG_DATABASE_DETAIL_GET_TYPE_HPP

#include <boost/algorithm/string.hpp>
#include <brig/database/column_detail.hpp>
#include <brig/database/detail/is_geometry_type.hpp>
#include <brig/database/global.hpp>
#include <locale>

namespace brig { namespace database { namespace detail {

inline Type get_type(DBMS sys, const column_detail& col)
{
  using namespace boost::algorithm;
  auto loc = std::locale::classic();
  if (is_geometry_type(sys, col)) return Geometry;
  else
    switch (sys)
    {
    case UnknownSystem:
    case SQLite:
      break;

    case DB2:
      if (!col.type.schema.empty() && !iequals(col.type.schema, "SYSIBM", loc)) return UnknownType;
      else if (icontains(col.type.name, "GRAPHIC", loc)) return String;
      break;

    case MS_SQL:
      if (iequals(col.type.name, "BIT", loc)) return Integer;
      else if (iequals(col.type.name, "DATETIME", loc)
            || iequals(col.type.name, "DATETIME2", loc)
            || iequals(col.type.name, "SMALLDATETIME", loc)) return DateTime;
      break;

    case MySQL:
      if (iequals(col.type.name, "FIXED", loc)) return col.scale == 0? Integer: Double;
      else if (iequals(col.type.name, "DATETIME", loc)) return DateTime;
      break;

    case Oracle:
      if (!col.type.schema.empty()) return UnknownType;
      else if (iequals(col.type.name, "LONG", loc)) return String;
      break;

    case Postgres:
      if (iequals(col.type.schema, "USER-DEFINED", loc)) return UnknownType;
      else if (icontains(col.type.name, "SERIAL", loc)) return Integer;
      break;
    }

  if (iequals(col.type.name, "DATE", loc)) return Date;
  else if (istarts_with(col.type.name, "TIMESTAMP", loc)) return DateTime;
  else if (icontains(col.type.name, "INT", loc)
        || istarts_with(col.type.name, "BOOL", loc)) return Integer;
  else if (icontains(col.type.name, "CHAR", loc)
        || icontains(col.type.name, "CLOB", loc)
        || icontains(col.type.name, "TEXT", loc)) return String;
  else if (icontains(col.type.name, "REAL", loc)
        || icontains(col.type.name, "FLOAT", loc)
        || icontains(col.type.name, "DOUBLE", loc)) return Double;
  else if (istarts_with(col.type.name, "DEC", loc)
        || istarts_with(col.type.name, "NUM", loc)) return col.scale == 0? Integer: Double;
  else return UnknownType;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_TYPE_HPP
