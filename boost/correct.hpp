// Andrew Naplavkov

#ifndef BRIG_BOOST_CORRECT_HPP
#define BRIG_BOOST_CORRECT_HPP

#include <brig/boost/detail/correct_visitor.hpp>
#include <brig/boost/geometry.hpp>

namespace brig { namespace boost {

inline geometry correct(const geometry& geom)
{
  detail::correct_visitor visitor;
  return ::boost::apply_visitor(visitor, geom);
}

} } // brig::boost

#endif // BRIG_BOOST_CORRECT_HPP
