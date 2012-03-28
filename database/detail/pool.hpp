// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_POOL_HPP
#define BRIG_DATABASE_DETAIL_POOL_HPP

#include <boost/circular_buffer.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/utility.hpp>
#include <brig/database/command.hpp>
#include <brig/database/command_allocator.hpp>
#include <brig/database/detail/threaded_command_allocator.hpp>
#include <brig/database/global.hpp>
#include <exception>
#include <memory>

namespace brig { namespace database { namespace detail {

template <bool Threading> class pool;

template <> class pool<false> : ::boost::noncopyable {
  std::shared_ptr<command_allocator> m_allocator;
  ::boost::circular_buffer<command*> m_commands;
public:
  explicit pool(std::shared_ptr<command_allocator> allocator) : m_allocator(allocator), m_commands(PoolSize)  {}
  virtual ~pool();
  command* allocate();
  void deallocate(command* cmd);
}; // pool<false>

inline pool<false>::~pool()
{
  for (size_t i(0); i < m_commands.size(); ++i)
    m_allocator->deallocate(m_commands[i]);
}

inline command* pool<false>::allocate()
{
  if (m_commands.empty()) return m_allocator->allocate();
  command* cmd(m_commands.back());
  m_commands.pop_back();
  return cmd;
}

inline void pool<false>::deallocate(command* cmd)
{
  if (!m_commands.full())
  {
    try
    {
      cmd->set_autocommit(true);
      m_commands.push_back(cmd);
      return;
    }
    catch (const std::exception&)  {}
  }
  m_allocator->deallocate(cmd);
} // pool<false>::

template <> class pool<true> : public pool<false> {
  ::boost::mutex m_mut;
public:
  explicit pool(std::shared_ptr<command_allocator> allocator) : pool<false>(std::make_shared<threaded_command_allocator>(allocator))  {}
  command* allocate()  { ::boost::lock_guard<typename ::boost::mutex> lck(m_mut); return pool<false>::allocate(); }
  void deallocate(command* cmd)  { ::boost::lock_guard<typename ::boost::mutex> lck(m_mut); pool<false>::deallocate(cmd); }
}; // pool<true>

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_POOL_HPP
