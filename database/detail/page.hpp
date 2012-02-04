// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_PAGE_HPP
#define BRIG_DATABASE_DETAIL_PAGE_HPP

#include <algorithm>
#include <brig/database/detail/rowset.hpp>
#include <brig/database/global.hpp>
#include <brig/database/variant.hpp>
#include <vector>

namespace brig { namespace database { namespace detail {

class page : public rowset {
  std::vector<std::vector<variant>> m_rows;
  size_t m_beg, m_end;
  static size_t next(size_t pos)  { return pos < PageSize? pos + 1: 0; }
public:
  page() : m_rows(PageSize + 1), m_beg(0), m_end(0)  {}
  bool empty() const  { return m_beg == m_end; }
  bool fill(rowset* rs);
  void reset()  { m_beg = m_end = 0; }
  static void swap(page& a, page& b);
  virtual bool fetch(std::vector<variant>& row);
}; // page

inline bool page::fill(rowset* rs)
{
  while (true)
  {
    const size_t end(next(m_end));
    if (m_beg == end) return true;
    if (!rs || !rs->fetch(m_rows[m_end])) return false;
    m_end = end;
  }
}

inline void page::swap(page& a, page& b)
{
  a.m_rows.swap(b.m_rows);
  std::swap(a.m_beg, b.m_beg);
  std::swap(a.m_end, b.m_end);
}

inline bool page::fetch(std::vector<variant>& row)
{
  if (m_beg == m_end) return false;
  row.swap(m_rows[m_beg]);
  m_beg = next(m_beg);
  return true;
} // page::

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_PAGE_HPP
