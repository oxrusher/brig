// Andrew Naplavkov

#ifndef BRIG_DATABASE_CUBRID_COMMAND_ALLOCATOR_HPP
#define BRIG_DATABASE_CUBRID_COMMAND_ALLOCATOR_HPP

#include <brig/database/command_allocator.hpp>
#include <brig/database/cubrid/detail/command.hpp>
#include <brig/database/cubrid/detail/lib.hpp>
#include <string>

namespace brig { namespace database { namespace cubrid {

class command_allocator : public brig::database::command_allocator {
  std::string m_ip, m_db, m_usr, m_pwd;
  int m_port;
public:
  command_allocator(const std::string& ip, int port, const std::string& db, const std::string& usr, const std::string& pwd)
    : m_ip(ip), m_db(db), m_usr(usr), m_pwd(pwd)
    , m_port(port)
    { detail::lib::singleton(); }
  virtual command* allocate()
    { return new detail::command(m_ip, m_port, m_db, m_usr, m_pwd); }
}; // command_allocator

} } } // brig::database::cubrid

#endif // BRIG_DATABASE_CUBRID_COMMAND_ALLOCATOR_HPP
