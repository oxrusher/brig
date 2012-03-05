// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_DETAIL_DEFINE_STRING_HPP
#define BRIG_DATABASE_ORACLE_DETAIL_DEFINE_STRING_HPP

#include <brig/database/oracle/detail/define.hpp>
#include <brig/database/oracle/detail/handles.hpp>
#include <brig/database/oracle/detail/lib.hpp>
#include <brig/database/variant.hpp>
#include <brig/unicode/transform.hpp>
#include <climits>
#include <string>

namespace brig { namespace database { namespace oracle { namespace detail {

class define_string : public define {
  std::u16string m_str;
  OCIInd m_ind;
public:
  define_string(handles* hnd, size_t order, ub2 size, ub1 cs_form);
  virtual void operator()(variant& var);
}; // define_string

inline define_string::define_string(handles* hnd, size_t order, ub2 size, ub1 cs_form) : m_str(size > 0? size: SHRT_MAX / sizeof(char16_t), 0), m_ind(OCI_IND_NULL)
{
  OCIDefine* def(0);
  hnd->check(lib::singleton().p_OCIDefineByPos(hnd->stmt, &def, hnd->err, ub4(order)
    , (void*)m_str.c_str(), ub4((m_str.size() + 1) * sizeof(char16_t)), SQLT_STR, &m_ind, 0, 0, OCI_DEFAULT));
  hnd->check(lib::singleton().p_OCIAttrSet(def, OCI_HTYPE_DEFINE, (void*)&cs_form, 0, OCI_ATTR_CHARSET_FORM, hnd->err));
}

inline void define_string::operator()(variant& var)
{
  if (OCI_IND_NULL == m_ind)
    var = null_t();
  else
    var = brig::unicode::transform<std::string>(m_str);
} // define_string::

} } } } // brig::database::oracle::detail

#endif // BRIG_DATABASE_ORACLE_DETAIL_DEFINE_STRING_HPP
