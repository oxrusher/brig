// Andrew Naplavkov

#ifndef BRIG_DATABASE_ROWSET_HPP
#define BRIG_DATABASE_ROWSET_HPP

#include <boost/utility.hpp>
#include <brig/database/variant.hpp>
#include <vector>

namespace brig { namespace database {

struct rowset : ::boost::noncopyable {
  virtual ~rowset()  {}
  virtual void columns(std::vector<std::string>& cols) = 0;
  virtual bool fetch(std::vector<variant>& row) = 0;
}; // rowset

} } // brig::database

#endif // BRIG_DATABASE_ROWSET_HPP
