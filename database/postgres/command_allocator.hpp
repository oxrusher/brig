// Andrew Naplavkov

#ifndef BRIG_DATABASE_POSTGRES_COMMAND_ALLOCATOR_HPP
#define BRIG_DATABASE_POSTGRES_COMMAND_ALLOCATOR_HPP

#include <brig/database/command_allocator.hpp>
#include <brig/database/postgres/detail/command.hpp>
#include <brig/database/postgres/detail/lib.hpp>
#include <string>

namespace brig { namespace database { namespace postgres {

class command_allocator : public brig::database::command_allocator {
  std::string m_host, m_db, m_usr, m_pwd;
  int m_port;
public:
  command_allocator(const std::string& host, int port, const std::string& db, const std::string& usr, const std::string& pwd)
    : m_host(host), m_db(db), m_usr(usr), m_pwd(pwd)
    , m_port(port)
    { detail::lib::singleton(); }
  virtual command* allocate()
    { return new detail::command(m_host, m_port, m_db, m_usr, m_pwd); }
}; // command_allocator

} } } // brig::database::postgres

#endif // BRIG_DATABASE_POSTGRES_COMMAND_ALLOCATOR_HPP
