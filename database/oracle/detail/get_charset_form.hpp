// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_DETAIL_GET_CHARSET_FORM_HPP
#define BRIG_DATABASE_ORACLE_DETAIL_GET_CHARSET_FORM_HPP

#include <brig/database/oracle/detail/lib.hpp>
#include <string>

namespace brig { namespace database { namespace oracle { namespace detail {

inline ub1 get_charset_form(const std::string& type_schema, const std::string& type_name)
{
  return (type_schema.empty() && (type_name == "CHAR" || type_name == "VARCHAR2" || type_name == "CLOB"))? SQLCS_IMPLICIT: SQLCS_NCHAR;
}

} } } } // brig::database::oracle::detail

#endif // BRIG_DATABASE_ORACLE_DETAIL_GET_CHARSET_FORM_HPP
