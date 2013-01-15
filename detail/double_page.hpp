// Andrew Naplavkov

#ifndef BRIG_DETAIL_DOUBLE_PAGE_HPP
#define BRIG_DETAIL_DOUBLE_PAGE_HPP

#include <boost/utility.hpp>
#include <brig/detail/page.hpp>
#include <brig/rowset.hpp>
#include <brig/variant.hpp>
#include <exception>
#include <vector>

namespace brig { namespace detail {

class double_page : ::boost::noncopyable {
  page m_front, m_back;
  bool m_sync, m_done;
  std::exception_ptr m_exc;

public:
  double_page() : m_sync(false), m_done(false)  {}
  void prefill(rowset* rs);

  void clear()  { m_front.clear(); m_sync = false; }
  bool empty() const  { return m_front.empty(); }
  bool fetch(std::vector<variant>& row)  { return m_front.fetch(row); }
  void fill(rowset* rs);
}; // double_page

inline void double_page::prefill(rowset* rs)
{
  if (!m_sync || m_done || !(m_exc == 0)) return;
  try
  {
    m_back.fill(rs);
    m_done = !m_back.full();
  }
  catch (const std::exception&)
  {
    m_back.clear();
    m_done = true;
    m_exc = std::current_exception();
  }
}
    
inline void double_page::fill(rowset* rs)
{
  if (!m_sync)
  {
    m_back.clear();
    m_back.fill(rs);
    m_done = !m_back.full();
    m_exc = std::exception_ptr();
    m_sync = true;
  }
  if (!(m_exc == 0)) std::rethrow_exception(std::move(m_exc));
  m_front.swap(m_back);
  page tmp; m_back.swap(tmp); // instead of clear() as windows garbage collector workaround
} // double_page::

} } // brig::detail

#endif // BRIG_DETAIL_DOUBLE_PAGE_HPP
