// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_DETAIL_BINDING_BLOB_HPP
#define BRIG_DATABASE_ORACLE_DETAIL_BINDING_BLOB_HPP

#include <brig/database/oracle/detail/binding.hpp>
#include <brig/database/oracle/detail/handles.hpp>
#include <brig/database/oracle/detail/lib.hpp>

namespace brig { namespace database { namespace oracle { namespace detail {

class binding_blob : public binding {
  void* m_data;
  ub4 m_len;
  static sb4 callback(void *ictxp, OCIBind *bindp, ub4 iter, ub4 index, void **bufpp, ub4 *alenp, ub1 *piecep, void **indpp);
public:
  binding_blob(handles* hnd, size_t order, void* data, ub4 len);
}; // binding_blob

inline binding_blob::binding_blob(handles* hnd, size_t order, void* data, ub4 len) : m_data(data), m_len(len)
{
  OCIBind* bnd(0);
  hnd->check(lib::singleton().p_OCIBindByPos(hnd->stmt, &bnd, hnd->err, ub4(order), 0, 0, SQLT_LBI, 0, 0, 0, 0, 0, OCI_DATA_AT_EXEC));
  hnd->check(lib::singleton().p_OCIBindDynamic(bnd, hnd->err, this, callback, 0, 0));
}

inline sb4 binding_blob::callback(void *ictxp, OCIBind *bindp, ub4 iter, ub4 index, void **bufpp, ub4 *alenp, ub1 *piecep, void **indpp)
{
  binding_blob* p_bnd(static_cast<binding_blob*>(ictxp));

  *bufpp = p_bnd->m_data;
  *alenp = p_bnd->m_len;
  *piecep = OCI_ONE_PIECE;
  *indpp = 0;

  return OCI_CONTINUE;
} // binding_blob::

} } } } // brig::database::oracle::detail

#endif // BRIG_DATABASE_ORACLE_DETAIL_BINDING_BLOB_HPP
