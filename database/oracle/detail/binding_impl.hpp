// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_DETAIL_BINDING_IMPL_HPP
#define BRIG_DATABASE_ORACLE_DETAIL_BINDING_IMPL_HPP

#include <brig/database/oracle/detail/binding.hpp>
#include <brig/database/oracle/detail/handles.hpp>
#include <brig/database/oracle/detail/lib.hpp>

namespace brig { namespace database { namespace oracle { namespace detail {

template <typename T, ub2 TargetType>
class binding_impl : public binding {
  T m_val;
  OCIInd m_ind;
  void bind(handles* hnd, size_t order);
public:
  binding_impl(handles* hnd, size_t order) :  m_val(), m_ind(OCI_IND_NULL)  { bind(hnd, order); }
  binding_impl(handles* hnd, size_t order, T val) : m_val(val), m_ind(sizeof(val))  { bind(hnd, order); }
}; // binding_impl

template <typename T, ub2 TargetType>
void binding_impl<T, TargetType>::bind(handles* hnd, size_t order)
{
  OCIBind* bnd(0);
  hnd->check(lib::singleton().p_OCIBindByPos(hnd->stmt, &bnd, hnd->err, ub4(order), &m_val, sizeof(m_val), TargetType, &m_ind, 0, 0, 0, 0, OCI_DEFAULT));
} // binding_impl::

} } } } // brig::database::oracle::detail

#endif // BRIG_DATABASE_ORACLE_DETAIL_BINDING_IMPL_HPP
