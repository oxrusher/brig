// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_SCHEMA_FILTER_HPP
#define BRIG_DATABASE_DETAIL_SQL_SCHEMA_FILTER_HPP

#include <brig/database/global.hpp>
#include <sstream>
#include <string>

namespace brig { namespace database { namespace detail {

inline void sql_schema_filter(DBMS sys, const std::string& col, std::ostringstream& stream)
{
  switch (sys)
  {
  default: break;
  case DB2: stream << col << " NOT IN ('APP','DB2GSE','DB2QP','NULLID','SQLJ','ST_INFORMTN_SCHEMA') AND " << col << " NOT LIKE 'SYS%'"; break;
  case MS_SQL: stream << col << " NOT IN ('guest','INFORMATION_SCHEMA','sys') AND " << col << " NOT LIKE 'db[_]%' AND " << col << " NOT LIKE 'sys[_]%'"; break;
  case MySQL: stream << col << " NOT IN ('information_schema','mysql','performance_schema')"; break;
  case Oracle: stream << col << " NOT IN ('ANONYMOUS','APEX_040000','CTXSYS','DBSNMP','DIP','DMSYS','EXFSYS','MDDATA','MDSYS','MGMT_VIEW','OLAPSYS','ORDPLUGINS','ORDSYS','OUTLN','SI_INFORMTN_SCHEMA','SYS','SYSMAN','TSMSYS','WKSYS','WMSYS','XDB')"; break; // sample schemas: HR,OE,PM,IX,SH
  case Postgres: stream << col << " NOT SIMILAR TO '(information\\_schema)|(pg\\_%)'"; break;
  }
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_SCHEMA_FILTER_HPP
