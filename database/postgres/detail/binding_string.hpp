// Andrew Naplavkov

#ifndef BRIG_DATABASE_POSTGRES_DETAIL_BINDING_STRING_HPP
#define BRIG_DATABASE_POSTGRES_DETAIL_BINDING_STRING_HPP

#include <brig/database/postgres/detail/binding.hpp>
#include <brig/database/postgres/detail/lib.hpp>
#include <string>

namespace brig { namespace database { namespace postgres { namespace detail {

class binding_string : public binding {
  const std::string& m_str;
public:
  binding_string(const std::string& str) : m_str(str)  {}
  virtual Oid type()  { return PG_TYPE_TEXT; }
  virtual const char* value()  { return m_str.c_str(); }
  virtual int length()  { return m_str.size(); }
  virtual int format()  { return 0; }
}; // binding_string

} } } } // brig::database::postgres::detail

#endif // BRIG_DATABASE_POSTGRES_DETAIL_BINDING_STRING_HPP
