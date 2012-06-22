// Andrew Naplavkov

#ifndef BRIG_DATABASE_COMMAND_ALLOCATOR_HPP
#define BRIG_DATABASE_COMMAND_ALLOCATOR_HPP

#include <boost/utility.hpp>
#include <brig/database/command.hpp>

namespace brig { namespace database {

struct command_allocator : ::boost::noncopyable {
  virtual ~command_allocator()  {}
  virtual command* allocate() = 0; // new
}; // command_allocator

} } // brig::database

#endif // BRIG_DATABASE_COMMAND_ALLOCATOR_HPP
