// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_THREADED_COMMAND_ALLOCATOR_HPP
#define BRIG_DATABASE_DETAIL_THREADED_COMMAND_ALLOCATOR_HPP

#include <brig/database/command_allocator.hpp>
#include <brig/database/detail/threaded_command.hpp>
#include <memory>

namespace brig { namespace database { namespace detail {

class threaded_command_allocator : public command_allocator {
  std::shared_ptr<command_allocator> m_allocator;
public:
  threaded_command_allocator(std::shared_ptr<command_allocator> allocator) : m_allocator(allocator)  {}
  virtual command* allocate()  { return new threaded_command(m_allocator); }
}; // threaded_command_allocator

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_THREADED_COMMAND_ALLOCATOR_HPP
