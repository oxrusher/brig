// Andrew Naplavkov

#ifndef BRIG_DATABASE_CONNECTION_HPP
#define BRIG_DATABASE_CONNECTION_HPP

#include <brig/database/command.hpp>
#include <brig/database/detail/deleter.hpp>
#include <brig/database/detail/link.hpp>
#include <brig/database/detail/linker.hpp>
#include <brig/database/detail/pool.hpp>
#include <brig/database/detail/sql_tables.hpp>
#include <brig/database/object.hpp>
#include <brig/database/variant.hpp>
#include <brig/string_cast.hpp>
#include <memory>
#include <vector>

namespace brig { namespace database {

template <bool Threading>
class connection {
  typedef detail::pool<Threading> pool_type;
  std::shared_ptr<pool_type> m_pl;
  std::shared_ptr<detail::link> get_link()  { return std::shared_ptr<detail::link>(m_pl->pull(), detail::deleter<pool_type>(m_pl)); }
public:
  explicit connection(std::shared_ptr<detail::linker> lkr) : m_pl(new pool_type(lkr))  {}
  std::shared_ptr<command> get_command()  { return get_link(); }
  void get_tables(std::vector<object>& tables);
}; // connection

template <bool Threading>
void connection<Threading>::get_tables(std::vector<object>& tables)
{
  auto lnk = get_link();
  lnk->exec(detail::sql_tables(lnk->system()));
  std::vector<variant> row;
  while (lnk->fetch(row))
    tables.push_back(object(string_cast<char>(row[0]), string_cast<char>(row[1])));
}

} } // brig::database

#endif // BRIG_DATABASE_CONNECTION_HPP
