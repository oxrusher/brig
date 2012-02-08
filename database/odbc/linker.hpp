// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_LINKER_HPP
#define BRIG_DATABASE_ODBC_LINKER_HPP

#include <brig/database/detail/link.hpp>
#include <brig/database/detail/linker.hpp>
#include <brig/database/odbc/detail/lib.hpp>
#include <brig/database/odbc/detail/link.hpp>
#include <string>

namespace brig { namespace database { namespace odbc {

class linker : public brig::database::detail::linker {
  std::string m_str;
public:
  linker(const std::string& str) : m_str(str)  { detail::lib::singleton(); }
  virtual brig::database::detail::link* create()  { return new detail::link(m_str); }
}; // linker

} } } // brig::database::odbc

#endif // BRIG_DATABASE_ODBC_LINKER_HPP
