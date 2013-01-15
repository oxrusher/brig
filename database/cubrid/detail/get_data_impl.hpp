// Andrew Naplavkov

#ifndef BRIG_DATABASE_CUBRID_DETAIL_GET_DATA_IMPL_HPP
#define BRIG_DATABASE_CUBRID_DETAIL_GET_DATA_IMPL_HPP

#include <brig/database/cubrid/detail/get_data.hpp>
#include <brig/database/cubrid/detail/lib.hpp>
#include <brig/variant.hpp>

namespace brig { namespace database { namespace cubrid { namespace detail {

template <typename T, T_CCI_A_TYPE Type>
struct get_data_impl : get_data {
  int operator()(int req, size_t col, variant& var) override;
}; // get_data_impl

template <typename T, T_CCI_A_TYPE Type>
int get_data_impl<T, Type>::operator()(int req, size_t col, variant& var)
{
  T data;
  int ind(-1);
  const int r(lib::singleton().p_cci_get_data(req, int(col + 1), Type, &data, &ind));
  if (lib::error(r) || ind < 0) var = null_t();
  else var = data;
  return r;
} // get_data_impl::

} } } } // brig::database::cubrid::detail

#endif // BRIG_DATABASE_CUBRID_DETAIL_GET_DATA_IMPL_HPP
