// Andrew Naplavkov

#ifndef BRIG_DETAIL_PAGE_HPP
#define BRIG_DETAIL_PAGE_HPP

#include <algorithm>
#include <array>
#include <boost/utility.hpp>
#include <brig/global.hpp>
#include <brig/rowset.hpp>
#include <brig/variant.hpp>
#include <vector>

namespace brig { namespace detail {

class page : ::boost::noncopyable { // ::boost::circular_buffer is slow
  std::array<std::vector<variant>, PageSize + 1> m_rows;
  size_t m_beg, m_end;

  static size_t next(size_t pos)  { return pos < PageSize? pos + 1: 0; }

public:
  page() : m_beg(0), m_end(0)  {}
  void swap(page& r);
  bool full() const  { return m_beg == next(m_end); }

  void clear()  { m_beg = m_end = 0; }
  bool empty() const  { return m_beg == m_end; }
  bool fetch(std::vector<variant>& row);
  void fill(rowset* rs);
}; // page

inline void page::swap(page& r)
{
  m_rows.swap(r.m_rows);
  std::swap(m_beg, r.m_beg);
  std::swap(m_end, r.m_end);
}

inline bool page::fetch(std::vector<variant>& row)
{
  if (m_beg == m_end) return false;
  row.swap(m_rows[m_beg]);
  m_beg = next(m_beg);
  return true;
}

inline void page::fill(rowset* rs)
{
  while (true)
  {
    const size_t end(next(m_end));
    if (m_beg == end || !rs || !rs->fetch(m_rows[m_end])) return;
    m_end = end;
  }
} // page::

} } // brig::detail

#endif // BRIG_DETAIL_PAGE_HPP
