// Andrew Naplavkov

#ifndef BRIG_DATABASE_POSTGRES_DETAIL_GET_VALUE_STRING_HPP
#define BRIG_DATABASE_POSTGRES_DETAIL_GET_VALUE_STRING_HPP

#include <brig/database/postgres/detail/get_value.hpp>
#include <brig/database/postgres/detail/lib.hpp>
#include <string>

namespace brig { namespace database { namespace postgres { namespace detail {

struct get_value_string : get_value {
  void operator()(PGresult* res, int row, int col, variant& var) override;
}; // get_value_string

inline void get_value_string::operator()(PGresult* res, int row, int col, variant& var)
{
  var = std::string
    ( lib::singleton().p_PQgetvalue(res, row, col)
    , lib::singleton().p_PQgetlength(res, row, col)
    );
} // get_value_string::

} } } } // brig::database::postgres::detail

#endif // BRIG_DATABASE_POSTGRES_DETAIL_GET_VALUE_STRING_HPP
