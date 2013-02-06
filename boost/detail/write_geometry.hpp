// Andrew Naplavkov

#ifndef BRIG_BOOST_DETAIL_WRITE_GEOMETRY_HPP
#define BRIG_BOOST_DETAIL_WRITE_GEOMETRY_HPP

#include <brig/detail/ogc.hpp>
#include <brig/boost/detail/write_line.hpp>
#include <brig/boost/detail/write_point.hpp>
#include <brig/boost/detail/write_polygon.hpp>
#include <brig/boost/detail/write_sequence.hpp>
#include <brig/boost/geometry.hpp>
#include <cstdint>

namespace brig { namespace boost { namespace detail {

template <typename OutputIterator>
void write(OutputIterator& iter, const geometry_collection& coll);

template <typename OutputIterator>
struct write_visitor : ::boost::static_visitor<void> {
  OutputIterator& iter;
  explicit write_visitor(OutputIterator& iter_) : iter(iter_)  {}
  template <typename T>
  void operator()(const T& r) const  { write<>(iter, r); }
  void operator()(const ::boost::recursive_wrapper<geometry_collection>& coll) const  { write<>(iter, coll.get()); }
}; // write_visitor

template <typename OutputIterator>
void write(OutputIterator& iter, const geometry& geom)
{
  ::boost::apply_visitor(write_visitor<OutputIterator>(iter), geom);
}

template <typename OutputIterator>
void write(OutputIterator& iter, const geometry_collection& coll)
{
  using namespace brig::detail;
  using namespace brig::detail::ogc;
  write_byte_order(iter);
  ogc::write<uint32_t>(iter, uint32_t(GeometryCollection));
  ogc::write<uint32_t>(iter, uint32_t(coll.size()));
  for (size_t i(0); i < coll.size(); ++i)
    write<>(iter, coll[i]);
};

} } } // brig::boost::detail

#endif // BRIG_BOOST_DETAIL_WRITE_GEOMETRY_HPP
