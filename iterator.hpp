// Andrew Naplavkov

#ifndef BRIG_ITERATOR_HPP
#define BRIG_ITERATOR_HPP

#include <brig/rowset.hpp>
#include <brig/variant.hpp>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <vector>

namespace brig {

class iterator  : public std::iterator<std::input_iterator_tag, std::vector<variant>> {
  std::shared_ptr<rowset> m_rs;
  std::vector<variant> m_row;
  void check() const  { if (!bool(m_rs)) throw std::out_of_range("iterator error"); }
  void fetch()  { if (!m_rs->fetch(m_row)) m_rs.reset(); }
public:
  explicit iterator(std::shared_ptr<rowset> rs = std::shared_ptr<rowset>()) : m_rs(rs)  { if (bool(m_rs)) fetch(); }
  bool operator !=(const iterator& r) const { return m_rs != r.m_rs; }
  iterator& operator ++()  { check(); fetch(); return *this; }
  std::vector<variant>& operator *()  { check(); return m_row; }
  std::vector<variant>* operator ->()  { check(); return &m_row; }
}; // iterator

} // brig

namespace std {

inline brig::iterator begin(std::shared_ptr<brig::rowset> rs)  { return brig::iterator(rs); }
inline brig::iterator end(std::shared_ptr<brig::rowset>)  { return brig::iterator(); }

} // std

#endif // BRIG_ITERATOR_HPP
