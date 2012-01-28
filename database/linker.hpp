// Andrew Naplavkov

#ifndef BRIG_DATABASE_LINKER_HPP
#define BRIG_DATABASE_LINKER_HPP

#include <boost/utility.hpp>
#include <brig/database/link.hpp>

namespace brig { namespace database {

struct linker : boost::noncopyable {
  virtual ~linker()  {}
  virtual link* create() = 0;
}; // linker

} } // brig::database

#endif // BRIG_DATABASE_LINKER_HPP
