// Andrew Naplavkov

#ifndef BRIG_DATABASE_POSTGRES_DETAIL_GET_VALUE_IMPL_HPP
#define BRIG_DATABASE_POSTGRES_DETAIL_GET_VALUE_IMPL_HPP

#include <boost/detail/endian.hpp>
#include <brig/database/postgres/detail/get_value.hpp>
#include <brig/database/postgres/detail/lib.hpp>
#include <brig/detail/copy.hpp>
#include <cstdint>

namespace brig { namespace database { namespace postgres { namespace detail {

template <typename T>
struct get_value_impl : get_value {
  void operator()(PGresult* res, int row, int col, variant& var) override;
}; // get_value_impl

template <typename T>
void get_value_impl<T>::operator()(PGresult* res, int row, int col, variant& var)
{
#if defined BOOST_LITTLE_ENDIAN
  T val;
  uint8_t *from((uint8_t*)lib::singleton().p_PQgetvalue(res, row, col)), *to((uint8_t*)&val);
  brig::detail::reverse_copy<T>(from, to);
  var = val;
#elif defined BOOST_BIG_ENDIAN
  var = *(const T*)lib::singleton().p_PQgetvalue(res, row, col);
#else
  #error byte order error
#endif
} // get_value_impl::

} } } } // brig::database::postgres::detail

#endif // BRIG_DATABASE_POSTGRES_DETAIL_GET_VALUE_IMPL_HPP
