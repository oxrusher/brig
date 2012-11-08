// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_DETAIL_DEFINE_BINARY_HPP
#define BRIG_DATABASE_ORACLE_DETAIL_DEFINE_BINARY_HPP

#include <brig/blob_t.hpp>
#include <brig/database/oracle/detail/define.hpp>
#include <brig/database/oracle/detail/handles.hpp>
#include <brig/database/oracle/detail/lib.hpp>
#include <brig/database/variant.hpp>
#include <climits>
#include <cstring>
#include <string>

namespace brig { namespace database { namespace oracle { namespace detail {

class define_binary : public define {
  blob_t m_blob;
  OCIInd m_ind;
  ub2 m_len;
public:
  define_binary(handles* hnd, size_t order, ub2 size);
  void operator()(variant& var) override;
}; // define_binary

inline define_binary::define_binary(handles* hnd, size_t order, ub2 size) : m_ind(OCI_IND_NULL), m_len(0)
{
  m_blob.resize(size > 0? size: SHRT_MAX, 0);
  OCIDefine* def(0);
  hnd->check(lib::singleton().p_OCIDefineByPos(hnd->stmt, &def, hnd->err, ub4(order)
    , (void*)m_blob.data(), ub4(m_blob.size()), SQLT_BIN, &m_ind, &m_len, 0, OCI_DEFAULT));
}

inline void define_binary::operator()(variant& var)
{
  if (OCI_IND_NULL == m_ind)
    var = null_t();
  else
  {
    const size_t size(m_len);
    var = blob_t();
    blob_t& blob = ::boost::get<blob_t>(var);
    blob.resize(size);
    memcpy(blob.data(), m_blob.data(), size);
  }
} // define_binary::

} } } } // brig::database::oracle::detail

#endif // BRIG_DATABASE_ORACLE_DETAIL_DEFINE_BINARY_HPP
