// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_ROWSET_HPP
#define BRIG_DATABASE_DETAIL_ROWSET_HPP

#include <boost/utility.hpp>
#include <brig/database/variant.hpp>
#include <vector>

namespace brig { namespace database { namespace detail {

struct rowset : boost::noncopyable {
  virtual ~rowset()  {}
  virtual bool fetch(std::vector<variant>& row) = 0;
}; // rowset

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_ROWSET_HPP
