// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_DETAIL_DEFINE_BLOB_HPP
#define BRIG_DATABASE_ORACLE_DETAIL_DEFINE_BLOB_HPP

#include <brig/blob_t.hpp>
#include <brig/database/oracle/detail/define.hpp>
#include <brig/database/oracle/detail/handles.hpp>
#include <brig/database/oracle/detail/lib.hpp>
#include <brig/database/variant.hpp>
#include <climits>
#include <cstring>

namespace brig { namespace database { namespace oracle { namespace detail {

class define_blob : public define {
  blob_t m_piece, m_result;
  ub4 m_len;
  OCIInd m_ind;

  static sb4 callback(dvoid *octxp, OCIDefine *defnp, ub4 iter, dvoid **bufpp, ub4 **alenpp, ub1 *piecep, dvoid **indpp, ub2 **rcodep);
  void read();

public:
  define_blob(handles* hnd, size_t order);
  virtual void operator()(variant& var);
}; // define_blob

inline define_blob::define_blob(handles* hnd, size_t order) : m_len(0), m_ind(OCI_IND_NULL)
{
  m_piece.resize(SHRT_MAX);
  OCIDefine* def(0);
  hnd->check(lib::singleton().p_OCIDefineByPos(hnd->stmt, &def, hnd->err, ub4(order), 0, INT_MAX, SQLT_LBI, 0, 0, 0, OCI_DYNAMIC_FETCH));
  hnd->check(lib::singleton().p_OCIDefineDynamic(def, hnd->err, this, callback));
}

inline sb4 define_blob::callback(void *octxp, OCIDefine*, ub4, void **bufpp, ub4 **alenpp, ub1 *piecep, void **indpp, ub2 **rcodep)
{
  define_blob* p_def(static_cast<define_blob*>(octxp));

  switch (*piecep)
  {
  case OCI_ONE_PIECE:
  case OCI_FIRST_PIECE:
    p_def->m_result.clear();
    *piecep = OCI_FIRST_PIECE;
    break;
  default:
    p_def->read();
    *piecep = OCI_NEXT_PIECE;
    break;
  }

  *bufpp = (void*)p_def->m_piece.data();
  p_def->m_len = ub4(p_def->m_piece.size());
  *alenpp = &p_def->m_len;
  *indpp = (void*)&p_def->m_ind;
  *rcodep = 0;

  return OCI_CONTINUE;
}

inline void define_blob::read()
{
  if (m_len > 0 && OCI_IND_NULL != m_ind)
  {
    const size_t size(m_result.size());
    m_result.resize(size + m_len);
    memcpy(m_result.data() + size, m_piece.data(), m_len);
    m_len = 0;
    m_ind = OCI_IND_NULL;
  }
}

inline void define_blob::operator()(variant& var)
{
  read();
  if (m_result.empty())
    var = null_t();
  else
  {
    var = blob_t();
    blob_t& blob = ::boost::get<blob_t>(var);
    blob.swap(m_result);
  }
} // define_blob::

} } } } // brig::database::oracle::detail

#endif // BRIG_DATABASE_ORACLE_DETAIL_DEFINE_BLOB_HPP
