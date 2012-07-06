// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_SCHEMA_FILTER_HPP
#define BRIG_DATABASE_DETAIL_SQL_SCHEMA_FILTER_HPP

#include <brig/database/global.hpp>
#include <string>

namespace brig { namespace database { namespace detail {

inline std::string sql_schema_filter(DBMS sys, const std::string& col)
{
  switch (sys)
  {
  default: return "";
  case DB2: return col + " NOT IN ('APP','DB2GSE','DB2QP','NULLID','SQLJ','ST_INFORMTN_SCHEMA') AND " + col + " NOT LIKE 'SYS%'";
  case MS_SQL: return col + " NOT IN ('guest','INFORMATION_SCHEMA','sys') AND " + col + " NOT LIKE 'db[_]%' AND " + col + " NOT LIKE 'sys[_]%'";
  case MySQL: return col + " NOT IN ('information_schema','mysql','performance_schema')";
  case Oracle: return col + " NOT IN ('ANONYMOUS','APEX_040000','CTXSYS','DBSNMP','DIP','DMSYS','EXFSYS','MDDATA','MDSYS','MGMT_VIEW','OLAPSYS','ORDPLUGINS','ORDSYS','OUTLN','SI_INFORMTN_SCHEMA','SYS','SYSMAN','TSMSYS','WKSYS','WMSYS','XDB')"; // sample schemas: HR,OE,PM,IX,SH
  case Postgres: return col + " NOT SIMILAR TO E'(information\\_schema)|(pg\\_%)'";
  }
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_SCHEMA_FILTER_HPP
