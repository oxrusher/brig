// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_DELETER_HPP
#define BRIG_DATABASE_DETAIL_DELETER_HPP

#include <brig/database/command.hpp>
#include <memory>

namespace brig { namespace database { namespace detail {

template <typename Pool>
class deleter {
  std::shared_ptr<Pool> m_pool;
public:
  explicit deleter(std::shared_ptr<Pool> pool) : m_pool(pool)  {}
  void operator()(command* cmd) const  { m_pool->deallocate(cmd); }
}; // deleter

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_DELETER_HPP
