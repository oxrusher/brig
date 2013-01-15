// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_DETAIL_DEFINE_IMPL_HPP
#define BRIG_DATABASE_ORACLE_DETAIL_DEFINE_IMPL_HPP

#include <brig/database/oracle/detail/define.hpp>
#include <brig/database/oracle/detail/handles.hpp>
#include <brig/database/oracle/detail/lib.hpp>
#include <brig/variant.hpp>

namespace brig { namespace database { namespace oracle { namespace detail {

template <typename T, ub2 TargetType>
class define_impl : public define {
  T m_val;
  OCIInd m_ind;
public:
  define_impl(handles* hnd, size_t order);
  void operator()(variant& var) override;
}; // define_impl

template <typename T, ub2 TargetType>
define_impl<T, TargetType>::define_impl(handles* hnd, size_t order) : m_ind(OCI_IND_NULL)
{
  OCIDefine* def(0);
  hnd->check(lib::singleton().p_OCIDefineByPos(hnd->stmt, &def, hnd->err, ub4(order), &m_val, sizeof(m_val), TargetType, &m_ind, 0, 0, OCI_DEFAULT));
}

template <typename T, ub2 TargetType>
void define_impl<T, TargetType>::operator()(variant& var)
{
  if (OCI_IND_NULL == m_ind)
    var = null_t();
  else
    var = m_val;
} // define_impl::

} } } } // brig::database::oracle::detail

#endif // BRIG_DATABASE_ORACLE_DETAIL_DEFINE_IMPL_HPP
