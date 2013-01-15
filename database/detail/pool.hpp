// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_POOL_HPP
#define BRIG_DATABASE_DETAIL_POOL_HPP

#include <boost/utility.hpp>
#include <brig/database/command.hpp>
#include <brig/database/command_allocator.hpp>
#include <brig/database/detail/threaded_command_allocator.hpp>
#include <brig/global.hpp>
#include <exception>
#include <memory>
#include <mutex>
#include <stack>

namespace brig { namespace database { namespace detail {

template <bool Threading> class pool;

template <> class pool<false> : ::boost::noncopyable {
  std::shared_ptr<command_allocator> m_allocator;
  std::stack<command*> m_commands;
public:
  explicit pool(std::shared_ptr<command_allocator> allocator) : m_allocator(allocator)  {}
  virtual ~pool();
  command* allocate();
  void deallocate(command* cmd);
}; // pool<false>

inline pool<false>::~pool()
{
  while (!m_commands.empty())
  {
    delete m_commands.top();
    m_commands.pop();
  }
}

inline command* pool<false>::allocate()
{
  if (m_commands.empty()) return m_allocator->allocate();
  command* cmd(m_commands.top());
  m_commands.pop();
  return cmd;
}

inline void pool<false>::deallocate(command* cmd)
{
  if (m_commands.size() < PoolSize)
  {
    try
    {
      cmd->set_autocommit(true);
      m_commands.push(cmd);
      return;
    }
    catch (const std::exception&)  {}
  }
  delete cmd;
} // pool<false>::

template <> class pool<true> : public pool<false> {
  std::mutex m_mut;
public:
  explicit pool(std::shared_ptr<command_allocator> allocator) : pool<false>(std::make_shared<threaded_command_allocator>(allocator))  {}
  command* allocate()  { std::lock_guard<typename std::mutex> lck(m_mut); return pool<false>::allocate(); }
  void deallocate(command* cmd)  { std::lock_guard<typename std::mutex> lck(m_mut); pool<false>::deallocate(cmd); }
}; // pool<true>

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_POOL_HPP
