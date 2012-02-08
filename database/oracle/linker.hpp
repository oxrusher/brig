// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_LINKER_HPP
#define BRIG_DATABASE_ORACLE_LINKER_HPP

#include <brig/database/detail/link.hpp>
#include <brig/database/detail/linker.hpp>
#include <brig/database/oracle/detail/lib.hpp>
#include <brig/database/oracle/detail/link.hpp>
#include <string>

namespace brig { namespace database { namespace oracle {

class linker : public brig::database::detail::linker {
  std::string m_srv, m_usr, m_pwd;
public:
  linker(const std::string& srv, const std::string& usr, const std::string& pwd) : m_srv(srv), m_usr(usr), m_pwd(pwd)  { detail::lib::singleton(); }
  virtual brig::database::detail::link* create()  { return new detail::link(m_srv, m_usr, m_pwd); }
}; // linker

} } } // brig::database::oracle

#endif // BRIG_DATABASE_ORACLE_LINKER_HPP
