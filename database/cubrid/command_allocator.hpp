// Andrew Naplavkov

#ifndef BRIG_DATABASE_CUBRID_COMMAND_ALLOCATOR_HPP
#define BRIG_DATABASE_CUBRID_COMMAND_ALLOCATOR_HPP

#include <brig/database/command_allocator.hpp>
#include <brig/database/cubrid/detail/command.hpp>
#include <brig/database/cubrid/detail/lib.hpp>
#include <brig/global.hpp>
#include <brig/string_cast.hpp>
#include <string>

namespace brig { namespace database { namespace cubrid {

class command_allocator : public brig::database::command_allocator {
  std::string m_url, m_usr, m_pwd;
public:
  command_allocator(const std::string& host, int port, const std::string& db, const std::string& usr, const std::string& pwd)
    : m_url("cci:CUBRID:" + host + ":" + string_cast<char>(port) + ":" + db + ":::?login_timeout=" + string_cast<char>(TimeoutSec * 1000)), m_usr(usr), m_pwd(pwd)
    { detail::lib::singleton(); }
  command* allocate() override
    { return new detail::command(m_url, m_usr, m_pwd); }
}; // command_allocator

} } } // brig::database::cubrid

#endif // BRIG_DATABASE_CUBRID_COMMAND_ALLOCATOR_HPP
