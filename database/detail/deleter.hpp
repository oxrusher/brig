// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_DELETER_HPP
#define BRIG_DATABASE_DETAIL_DELETER_HPP

#include <brig/database/detail/link.hpp>
#include <memory>

namespace brig { namespace database { namespace detail {

template <typename Pool>
class deleter {
  std::shared_ptr<Pool> m_pl;
public:
  explicit deleter(std::shared_ptr<Pool> pl) : m_pl(pl)  {}
  void operator()(link* lnk) const  { m_pl->push(lnk); }
}; // deleter

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_DELETER_HPP
