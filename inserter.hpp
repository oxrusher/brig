// Andrew Naplavkov

#ifndef BRIG_INSERTER_HPP
#define BRIG_INSERTER_HPP

#include <boost/utility.hpp>
#include <brig/variant.hpp>
#include <vector>

namespace brig {

struct inserter : ::boost::noncopyable {
  virtual ~inserter()  {}
  virtual void insert(std::vector<variant>& row) = 0;
  virtual void flush() = 0;
}; // inserter

} // brig

#endif // BRIG_INSERTER_HPP
