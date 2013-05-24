// Andrew Naplavkov

#ifndef BRIG_ROWSET_ITERATOR_HPP
#define BRIG_ROWSET_ITERATOR_HPP

#include <brig/rowset.hpp>
#include <brig/variant.hpp>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <vector>

namespace brig {

class rowset_iterator  : public std::iterator<std::input_iterator_tag, std::vector<variant>> {
  std::shared_ptr<rowset> m_rs;
  std::vector<variant> m_row;
  void check() const  { if (!bool(m_rs)) throw std::out_of_range("rowset iterator error"); }
  void fetch()  { if (!m_rs->fetch(m_row)) m_rs.reset(); }
public:
  explicit rowset_iterator(std::shared_ptr<rowset> rs = std::shared_ptr<rowset>()) : m_rs(rs)  { if (bool(m_rs)) fetch(); }
  bool operator ==(const rowset_iterator& r) const { return m_rs == r.m_rs; }
  bool operator !=(const rowset_iterator& r) const { return m_rs != r.m_rs; }
  rowset_iterator& operator ++()  { check(); fetch(); return *this; }
  rowset_iterator operator ++(int) { rowset_iterator tmp(*this); operator++(); return tmp; }
  std::vector<variant>& operator *()  { check(); return m_row; }
  std::vector<variant>* operator ->()  { check(); return &m_row; }
}; // rowset_iterator

} // brig

namespace std {

inline brig::rowset_iterator begin(std::shared_ptr<brig::rowset> rs)  { return brig::rowset_iterator(rs); }
inline brig::rowset_iterator end(std::shared_ptr<brig::rowset>)  { return brig::rowset_iterator(); }

} // std

#endif // BRIG_ROWSET_ITERATOR_HPP
