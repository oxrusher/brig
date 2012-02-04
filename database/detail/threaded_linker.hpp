// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_THREADED_LINKER_HPP
#define BRIG_DATABASE_DETAIL_THREADED_LINKER_HPP

#include <brig/database/detail/link.hpp>
#include <brig/database/detail/linker.hpp>
#include <brig/database/detail/threaded_link.hpp>
#include <memory>

namespace brig { namespace database { namespace detail {

class threaded_linker : public linker {
  std::shared_ptr<linker> m_lkr;
public:
  threaded_linker(std::shared_ptr<linker> lkr) : m_lkr(lkr)  {}
  virtual link* create()  { return new threaded_link(m_lkr); }
}; // threaded_linker

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_THREADED_LINKER_HPP
