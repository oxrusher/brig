// Andrew Naplavkov

#ifndef BRIG_BOOST_DETAIL_ENVELOPE_VISITOR_HPP
#define BRIG_BOOST_DETAIL_ENVELOPE_VISITOR_HPP

#include <boost/geometry/algorithms/envelope.hpp>
#include <boost/geometry/algorithms/expand.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <brig/boost/geometry.hpp>
#include <stdexcept>

namespace brig { namespace boost { namespace detail {

template <typename Single>
box envelope_single(const Single& s)
{
  box res;
  ::boost::geometry::envelope(s, res);
  return res;
}

template <typename Multi>
box envelope_multi(const Multi& m)
{
  box res;
  auto iter(std::begin(m)), end(std::end(m));
  if (iter == end) throw std::runtime_error("envelope error");
  ::boost::geometry::envelope(*iter, res); ++iter;
  for (; iter != end; ++iter)
  {
    box env;
    ::boost::geometry::envelope(*iter, env);
    ::boost::geometry::expand(res, env);
  }
  return res;
}

struct envelope_visitor : ::boost::static_visitor<box> {
  box operator()(const point& r) const  { return envelope_single(r); }
  box operator()(const linestring& r) const  { return envelope_single(r); }
  box operator()(const polygon& r) const  { return envelope_single(r); }
  box operator()(const multi_point& r) const  { return envelope_multi(r); }
  box operator()(const multi_linestring& r) const  { return envelope_multi(r); }
  box operator()(const multi_polygon& r) const  { return envelope_multi(r); }
  box operator()(const ::boost::recursive_wrapper<geometry_collection>&) const;
}; // envelope_visitor

inline box envelope_visitor::operator()(const ::boost::recursive_wrapper<geometry_collection>& c) const
{
  auto iter(std::begin(c.get())), end(std::end(c.get()));
  if (iter == end) throw std::runtime_error("envelope error");
  envelope_visitor visitor;
  box res(::boost::apply_visitor(visitor, *iter)); ++iter;
  for (; iter != end; ++iter)
    ::boost::geometry::expand(res, ::boost::apply_visitor(visitor, *iter));
  return res;
}

} } } // brig::boost::detail

#endif // BRIG_BOOST_DETAIL_ENVELOPE_VISITOR_HPP
