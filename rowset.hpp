// Andrew Naplavkov

#ifndef BRIG_ROWSET_HPP
#define BRIG_ROWSET_HPP

#include <boost/utility.hpp>
#include <brig/variant.hpp>
#include <string>
#include <vector>

namespace brig {

struct rowset : ::boost::noncopyable {
  virtual ~rowset()  {}
  virtual std::vector<std::string> columns() = 0;
  virtual bool fetch(std::vector<variant>& row) = 0;
}; // rowset

} // brig

#endif // BRIG_ROWSET_HPP
