// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_DETAIL_GET_CHARSET_FORM_HPP
#define BRIG_DATABASE_ORACLE_DETAIL_GET_CHARSET_FORM_HPP

#include <boost/algorithm/string.hpp>
#include <brig/database/object.hpp>
#include <brig/database/oracle/detail/lib.hpp>
#include <locale>
#include <string>

namespace brig { namespace database { namespace oracle { namespace detail {

inline ub1 get_charset_form(const object& type)
{
  using namespace boost::algorithm;
  auto loc = std::locale::classic();
  if (type.schema.empty() &&
    ( icontains(type.name, "CHAR", loc)
   || icontains(type.name, "VARCHAR2", loc)
   || icontains(type.name, "CLOB", loc)))
    return SQLCS_IMPLICIT;
  else
    return SQLCS_NCHAR;
}

} } } } // brig::database::oracle::detail

#endif // BRIG_DATABASE_ORACLE_DETAIL_GET_CHARSET_FORM_HPP
