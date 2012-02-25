// Andrew Naplavkov

#ifndef BRIG_DATABASE_SQLITE_COMMAND_ALLOCATOR_HPP
#define BRIG_DATABASE_SQLITE_COMMAND_ALLOCATOR_HPP

#include <brig/database/command_allocator.hpp>
#include <brig/database/sqlite/detail/db_handle.hpp>
#include <brig/database/sqlite/detail/command.hpp>
#include <brig/database/sqlite/detail/lib.hpp>
#include <memory>
#include <string>

namespace brig { namespace database { namespace sqlite {

class command_allocator : public brig::database::command_allocator {
  std::string m_file;
  std::shared_ptr<detail::db_handle> m_mem;
  std::shared_ptr<detail::db_handle> db() const  { return m_mem? m_mem: std::make_shared<detail::db_handle>(m_file); }
public:
  command_allocator(const std::string& file) : m_file(file)  { detail::lib::singleton(); if (":memory:" == m_file) m_mem = std::make_shared<detail::db_handle>(m_file); }
  virtual command* allocate()  { return new detail::command(db()); }
  void load_backup(command_allocator& from)  { db()->load_backup(*from.db()); }
}; // command_allocator

} } } // brig::database::sqlite

#endif // BRIG_DATABASE_SQLITE_COMMAND_ALLOCATOR_HPP
