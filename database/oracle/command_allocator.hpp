// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_COMMAND_ALLOCATOR_HPP
#define BRIG_DATABASE_ORACLE_COMMAND_ALLOCATOR_HPP

#include <brig/database/command_allocator.hpp>
#include <brig/database/oracle/detail/command.hpp>
#include <brig/string_cast.hpp>
#include <string>

namespace brig { namespace database { namespace oracle {

class command_allocator : public brig::database::command_allocator {
  std::string m_srv, m_usr, m_pwd;
public:
  command_allocator(const std::string& host, int port, const std::string& db, const std::string& usr, const std::string& pwd)
    : m_srv("//" + host + ":" + string_cast<char>(port) + "/" + db), m_usr(usr), m_pwd(pwd)
    {}
  command* allocate() override
    { return new detail::command(m_srv, m_usr, m_pwd); }
}; // command_allocator

} } } // brig::database::oracle

#endif // BRIG_DATABASE_ORACLE_COMMAND_ALLOCATOR_HPP
