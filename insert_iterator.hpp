// Andrew Naplavkov

#ifndef BRIG_INSERT_ITERATOR_HPP
#define BRIG_INSERT_ITERATOR_HPP

#include <brig/inserter.hpp>
#include <brig/variant.hpp>
#include <iterator>
#include <memory>
#include <vector>

namespace brig {

class insert_iterator : public std::iterator<std::output_iterator_tag, void, void, void, void> {
  std::shared_ptr<inserter> m_ins;
  size_t m_flush_size, m_unflushed;
  bool m_ok;

public:
  explicit insert_iterator(std::shared_ptr<inserter> ins, size_t flush_size = 0)
    : m_ins(ins), m_flush_size(flush_size), m_unflushed(0), m_ok(true)
    {}
  ~insert_iterator()
    { if (m_ok) m_ins->flush(); }
  insert_iterator& operator *()
    { return *this; }
  insert_iterator& operator ++()
    { return *this; }
  insert_iterator operator ++(int)
    { return *this; }
  insert_iterator& operator =(std::vector<variant>& row)
    {
      m_ok = false;
      m_ins->insert(row);
      ++m_unflushed;
      if (m_unflushed == m_flush_size)
      {
        m_ins->flush();
        m_unflushed = 0;
      }
      m_ok = true;
      return *this;
    }
}; // insert_iterator

} // brig

#endif // BRIG_INSERT_ITERATOR_HPP
