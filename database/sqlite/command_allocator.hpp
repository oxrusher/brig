// Andrew Naplavkov

#ifndef BRIG_DATABASE_SQLITE_COMMAND_ALLOCATOR_HPP
#define BRIG_DATABASE_SQLITE_COMMAND_ALLOCATOR_HPP

#include <brig/database/command_allocator.hpp>
#include <brig/database/sqlite/detail/command.hpp>
#include <brig/database/sqlite/detail/lib.hpp>
#include <string>

namespace brig { namespace database { namespace sqlite {

class command_allocator : public brig::database::command_allocator {
  std::string m_file;
public:
  command_allocator(const std::string& file) : m_file(file)  { detail::lib::singleton(); }
  command* allocate() override  { return new detail::command(m_file); }
}; // command_allocator

} } } // brig::database::sqlite

#endif // BRIG_DATABASE_SQLITE_COMMAND_ALLOCATOR_HPP
