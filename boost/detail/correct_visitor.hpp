// Andrew Naplavkov

#ifndef BRIG_BOOST_DETAIL_CORRECT_VISITOR_HPP
#define BRIG_BOOST_DETAIL_CORRECT_VISITOR_HPP

#include <boost/geometry/geometry.hpp>
#include <brig/boost/geometry.hpp>

namespace brig { namespace boost { namespace detail {

class correct_visitor : public ::boost::static_visitor<geometry> {
  template <typename T>
  geometry correct_impl(const T&) const;
public:
  geometry operator()(const point& r) const  { return r; }
  geometry operator()(const linestring& r) const  { return r; }
  geometry operator()(const polygon& r) const  { return correct_impl(r); }
  geometry operator()(const multi_point& r) const  { return r; }
  geometry operator()(const multi_linestring& r) const  { return r; }
  geometry operator()(const multi_polygon& r) const  { return correct_impl(r); }
  geometry operator()(const ::boost::recursive_wrapper<geometry_collection>&) const;
}; // correct_visitor

template <typename T>
geometry correct_visitor::correct_impl(const T& r) const
{
  T res(r);
  ::boost::geometry::correct(res);
  return res;
}

inline geometry correct_visitor::operator()(const ::boost::recursive_wrapper<geometry_collection>& r) const
{
  geometry_collection res;
  detail::correct_visitor visitor;
  for (const auto& g: r.get())
    res.push_back( ::boost::apply_visitor(visitor, g) );
  return res;
} // correct_visitor::

} } } // brig::boost::detail

#endif // BRIG_BOOST_DETAIL_CORRECT_VISITOR_HPP
