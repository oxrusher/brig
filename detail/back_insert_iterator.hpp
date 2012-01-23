// Andrew Naplavkov

#ifndef BRIG_DETAIL_BACK_INSERT_ITERATOR_HPP
#define BRIG_DETAIL_BACK_INSERT_ITERATOR_HPP

#include <iterator>

namespace brig { namespace detail {

template <typename Container>
struct back_insert_iterator : std::back_insert_iterator<Container>
{
  typedef typename Container::value_type value_type;
  back_insert_iterator(Container& cont) : std::back_insert_iterator<Container>(cont)  {}
}; // back_insert_iterator

template<class Container>
back_insert_iterator<Container> back_inserter(Container& cont)
{
  return back_insert_iterator<Container>(cont);
}

} } // brig::detail

#endif // BRIG_DETAIL_BACK_INSERT_ITERATOR_HPP
