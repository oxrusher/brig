// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_DETAIL_BINDING_STRING_HPP
#define BRIG_DATABASE_ORACLE_DETAIL_BINDING_STRING_HPP

#include <brig/database/oracle/detail/binding.hpp>
#include <brig/database/oracle/detail/handles.hpp>
#include <brig/database/oracle/detail/lib.hpp>
#include <brig/unicode/transform.hpp>
#include <stdexcept>
#include <string>

namespace brig { namespace database { namespace oracle { namespace detail {

class binding_string : public binding {
  std::u16string m_str;
  OCIInd m_ind;
public:
  binding_string(handles* hnd, size_t order, const std::string& str, ub1 cs_form);
}; // binding_string

inline binding_string::binding_string(handles* hnd, size_t order, const std::string& str, ub1 cs_form) : m_str(brig::unicode::transform<std::u16string>(str))
{
  const size_t size((m_str.size() + 1) * sizeof(char16_t));
  if (size > SHRT_MAX) throw std::runtime_error("OCI type error");
  m_ind = OCIInd(size);
  OCIBind* bnd(0);
  hnd->check(lib::singleton().p_OCIBindByPos(hnd->stmt, &bnd, hnd->err, ub4(order), (void*)m_str.c_str(), m_ind, SQLT_STR, &m_ind, 0, 0, 0, 0, OCI_DEFAULT));
  hnd->check(lib::singleton().p_OCIAttrSet(bnd, OCI_HTYPE_BIND, (void*)&cs_form, 0, OCI_ATTR_CHARSET_FORM, hnd->err));
} // binding_string::

} } } } // brig::database::oracle::detail

#endif // BRIG_DATABASE_ORACLE_DETAIL_BINDING_STRING_HPP
