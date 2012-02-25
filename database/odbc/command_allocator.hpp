// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_COMMAND_ALLOCATOR_HPP
#define BRIG_DATABASE_ODBC_COMMAND_ALLOCATOR_HPP

#include <brig/database/command_allocator.hpp>
#include <brig/database/odbc/detail/command.hpp>
#include <brig/database/odbc/detail/lib.hpp>
#include <string>

namespace brig { namespace database { namespace odbc {

class command_allocator : public brig::database::command_allocator {
  std::string m_str;
public:
  command_allocator(const std::string& str) : m_str(str)  { detail::lib::singleton(); }
  virtual command* allocate()  { return new detail::command(m_str); }
}; // command_allocator

} } } // brig::database::odbc

#endif // BRIG_DATABASE_ODBC_COMMAND_ALLOCATOR_HPP
