// Andrew Naplavkov

#ifndef BRIG_DATABASE_SQLITE_LINKER_HPP
#define BRIG_DATABASE_SQLITE_LINKER_HPP

#include <brig/database/detail/link.hpp>
#include <brig/database/detail/linker.hpp>
#include <brig/database/sqlite/detail/db_handle.hpp>
#include <brig/database/sqlite/detail/lib.hpp>
#include <brig/database/sqlite/detail/link.hpp>
#include <memory>
#include <string>

namespace brig { namespace database { namespace sqlite {

class linker : public brig::database::detail::linker {
  std::string m_file;
  std::shared_ptr<detail::db_handle> m_mem;
  std::shared_ptr<detail::db_handle> db() const  { return m_mem? m_mem: std::make_shared<detail::db_handle>(m_file); }
public:
  linker(const std::string& file) : m_file(file)  { detail::lib::singleton(); if (":memory:" == m_file) m_mem = std::make_shared<detail::db_handle>(m_file); }
  virtual brig::database::detail::link* create()  { return new detail::link(db()); }
  void load(linker& from)  { db()->load(*from.db()); }
}; // linker

} } } // brig::database::sqlite

#endif // BRIG_DATABASE_SQLITE_LINKER_HPP
