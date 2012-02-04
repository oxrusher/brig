// Andrew Naplavkov

#ifndef BRIG_DATABASE_SQLITE_LINKER_HPP
#define BRIG_DATABASE_SQLITE_LINKER_HPP

#include <brig/database/detail/link.hpp>
#include <brig/database/detail/linker.hpp>
#include <brig/database/sqlite/detail/connection.hpp>
#include <brig/database/sqlite/detail/link.hpp>
#include <memory>
#include <string>

namespace brig { namespace database { namespace sqlite {

class linker : public brig::database::detail::linker {
  std::string m_file;
  std::shared_ptr<detail::connection> m_dbc;
  std::shared_ptr<detail::connection> get_dbc() const  { return m_dbc? m_dbc: std::shared_ptr<detail::connection>(new detail::connection(m_file)); }
public:
  linker(const std::string& file) : m_file(file)  { if (":memory:" == m_file) m_dbc = std::shared_ptr<detail::connection>(new detail::connection(m_file)); }
  virtual brig::database::detail::link* create()  { return new detail::link(get_dbc()); }
  static void backup(linker& from, linker& to)  { to.get_dbc()->restore(*from.get_dbc()); }
}; // linker

} } } // brig::database::sqlite

#endif // BRIG_DATABASE_SQLITE_LINKER_HPP
