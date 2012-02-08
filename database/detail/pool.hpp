// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_POOL_HPP
#define BRIG_DATABASE_DETAIL_POOL_HPP

#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/circular_buffer.hpp>
#include <boost/utility.hpp>
#include <brig/database/detail/link.hpp>
#include <brig/database/detail/linker.hpp>
#include <brig/database/detail/threaded_linker.hpp>
#include <brig/database/global.hpp>
#include <memory>

namespace brig { namespace database { namespace detail {

template <bool Threading> class pool;

template <> class pool<false> : boost::noncopyable {
  std::shared_ptr<linker> m_lkr;
  boost::circular_buffer<link*> m_lnks;
public:
  explicit pool(std::shared_ptr<linker> lkr) : m_lkr(lkr), m_lnks(PoolSize)  {}
  virtual ~pool();
  link* pull();
  void push(link* lnk);
}; // pool<false>

pool<false>::~pool()
{
  for (size_t i(0); i < m_lnks.size(); ++i)
    delete m_lnks[i];
}

inline link* pool<false>::pull()
{
  if (m_lnks.empty()) return m_lkr->create();
  link* lnk(m_lnks.back());
  m_lnks.pop_back();
  return lnk;
}

void pool<false>::push(link* lnk)
{
  if (m_lnks.full()) delete lnk;
  else m_lnks.push_back(lnk);
} // pool<false>::

template <> class pool<true> : public pool<false> {
  boost::mutex m_mut;
public:
  explicit pool(std::shared_ptr<linker> lkr) : pool<false>(std::make_shared<threaded_linker>(lkr))  {}
  link* pull()  { boost::lock_guard<boost::mutex> lck(m_mut); return pool<false>::pull(); }
  void push(link* lnk)  { boost::lock_guard<boost::mutex> lck(m_mut); pool<false>::push(lnk); }
}; // pool<true>

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_POOL_HPP
