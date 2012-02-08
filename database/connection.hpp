// Andrew Naplavkov

#ifndef BRIG_DATABASE_CONNECTION_HPP
#define BRIG_DATABASE_CONNECTION_HPP

#include <boost/utility.hpp>
#include <brig/database/command.hpp>
#include <brig/database/detail/deleter.hpp>
#include <brig/database/detail/linker.hpp>
#include <brig/database/detail/pool.hpp>
#include <memory>

namespace brig { namespace database {

template <bool Threading>
class connection : boost::noncopyable {
  typedef detail::pool<Threading> pool_type;
  typedef detail::deleter<pool_type> deleter_type;
  std::shared_ptr<pool_type> m_pl;
public:
  explicit connection(std::shared_ptr<detail::linker> lkr) : m_pl(new pool_type(lkr))  {}
  std::shared_ptr<command> get_command()  { return std::shared_ptr<detail::link>(m_pl->pull(), deleter_type(m_pl)); }
}; // connection

} } // brig::database

#endif // BRIG_DATABASE_CONNECTION_HPP
