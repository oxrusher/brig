// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_DETAIL_GET_CHARSET_FORM_HPP
#define BRIG_DATABASE_ORACLE_DETAIL_GET_CHARSET_FORM_HPP

#include <brig/database/oracle/detail/lib.hpp>
#include <brig/identifier.hpp>
#include <string>

namespace brig { namespace database { namespace oracle { namespace detail {

inline ub1 get_charset_form(const identifier& type_lcase)
{
  using namespace std;

  if (type_lcase.schema.empty() && (type_lcase.name.find("nchar") != string::npos || type_lcase.name.find("nvarchar2") != string::npos || type_lcase.name.find("nclob") != string::npos))
    return SQLCS_NCHAR;
  else
    return SQLCS_IMPLICIT;
}

} } } } // brig::database::oracle::detail

#endif // BRIG_DATABASE_ORACLE_DETAIL_GET_CHARSET_FORM_HPP
