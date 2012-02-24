// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_DETAIL_GET_CHARSET_FORM_HPP
#define BRIG_DATABASE_ORACLE_DETAIL_GET_CHARSET_FORM_HPP

#include <brig/database/object.hpp>
#include <brig/database/oracle/detail/lib.hpp>
#include <string>

namespace brig { namespace database { namespace oracle { namespace detail {

inline ub1 get_charset_form(const object& lower_case_type)
{
  if (lower_case_type.schema.empty() &&
    ( lower_case_type.name.find("char") != std::string::npos
   || lower_case_type.name.find("varchar2") != std::string::npos
   || lower_case_type.name.find("clob") != std::string::npos)
    )
    return SQLCS_IMPLICIT;
  else
    return SQLCS_NCHAR;
}

} } } } // brig::database::oracle::detail

#endif // BRIG_DATABASE_ORACLE_DETAIL_GET_CHARSET_FORM_HPP
