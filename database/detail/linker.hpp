// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_LINKER_HPP
#define BRIG_DATABASE_DETAIL_LINKER_HPP

#include <boost/utility.hpp>
#include <brig/database/detail/link.hpp>

namespace brig { namespace database { namespace detail {

struct linker : boost::noncopyable {
  virtual ~linker()  {}
  virtual link* create() = 0;
}; // linker

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_LINKER_HPP
