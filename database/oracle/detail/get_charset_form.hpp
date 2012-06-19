// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_DETAIL_GET_CHARSET_FORM_HPP
#define BRIG_DATABASE_ORACLE_DETAIL_GET_CHARSET_FORM_HPP

#include <brig/database/identifier.hpp>
#include <brig/database/oracle/detail/lib.hpp>
#include <string>

namespace brig { namespace database { namespace oracle { namespace detail {

inline ub1 get_charset_form(const identifier& dbms_type_lcase)
{
  if (dbms_type_lcase.schema.empty() && (dbms_type_lcase.name.find("nchar") != std::string::npos || dbms_type_lcase.name.find("nvarchar2") != std::string::npos || dbms_type_lcase.name.find("nclob") != std::string::npos))
    return SQLCS_NCHAR;
  else
    return SQLCS_IMPLICIT;
}

} } } } // brig::database::oracle::detail

#endif // BRIG_DATABASE_ORACLE_DETAIL_GET_CHARSET_FORM_HPP
