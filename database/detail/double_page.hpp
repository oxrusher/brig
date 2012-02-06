// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_DOUBLE_PAGE_HPP
#define BRIG_DATABASE_DETAIL_DOUBLE_PAGE_HPP

#include <boost/utility.hpp>
#include <brig/database/detail/page.hpp>
#include <brig/database/detail/rowset.hpp>
#include <brig/database/variant.hpp>
#include <vector>

namespace brig { namespace database { namespace detail {

class double_page : boost::noncopyable {
  page m_front, m_back;
  bool m_sync, m_done;

public:
  double_page() : m_sync(false), m_done(false)  {}
  void prefill(rowset* rs);

  void clear()  { m_front.clear(); m_sync = false; }
  bool empty() const  { return m_front.empty(); }
  bool fetch(std::vector<variant>& row)  { return m_front.fetch(row); }
  void fill(rowset* rs);
}; // double_page

void double_page::prefill(rowset* rs)
{
  if (m_sync && !m_done)
  {
    m_back.fill(rs);
    m_done = !m_back.full();
  }
}
    
void double_page::fill(rowset* rs)
{
  if (!m_sync)
  {
    m_back.clear();
    m_back.fill(rs);
    m_sync = true;
    m_done = !m_back.full();
  }
  m_front.swap(m_back);
} // double_page::

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_DOUBLE_PAGE_HPP
