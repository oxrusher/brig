// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_DETAIL_GET_CHARSET_FORM_HPP
#define BRIG_DATABASE_ORACLE_DETAIL_GET_CHARSET_FORM_HPP

#include <brig/database/object.hpp>
#include <brig/database/oracle/detail/lib.hpp>
#include <string>

namespace brig { namespace database { namespace oracle { namespace detail {

inline ub1 get_charset_form(const object& case_folded_type)
{
  if (case_folded_type.schema.empty() &&
    ( case_folded_type.name.find("char") != std::string::npos
   || case_folded_type.name.find("varchar2") != std::string::npos
   || case_folded_type.name.find("clob") != std::string::npos)
    )
    return SQLCS_IMPLICIT;
  else
    return SQLCS_NCHAR;
}

} } } } // brig::database::oracle::detail

#endif // BRIG_DATABASE_ORACLE_DETAIL_GET_CHARSET_FORM_HPP
