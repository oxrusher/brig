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
void write(OutputIterator& itr, const geometry_collection& coll);

template <typename OutputIterator>
struct write_visitor : ::boost::static_visitor<void> {
  OutputIterator& itr;
  explicit write_visitor(OutputIterator& iter_) : itr(iter_)  {}
  template <typename T>
  void operator()(const T& r) const  { write<>(itr, r); }
  void operator()(const ::boost::recursive_wrapper<geometry_collection>& coll) const  { write<>(itr, coll.get()); }
}; // write_visitor

template <typename OutputIterator>
void write(OutputIterator& itr, const geometry& geom)
{
  ::boost::apply_visitor(write_visitor<OutputIterator>(itr), geom);
}

template <typename OutputIterator>
void write(OutputIterator& itr, const geometry_collection& coll)
{
  using namespace brig::detail;
  using namespace brig::detail::ogc;
  write_byte_order(itr);
  ogc::write<uint32_t>(itr, uint32_t(GeometryCollection));
  ogc::write<uint32_t>(itr, uint32_t(coll.size()));
  for (size_t i(0); i < coll.size(); ++i)
    write<>(itr, coll[i]);
};

} } } // brig::boost::detail

#endif // BRIG_BOOST_DETAIL_WRITE_GEOMETRY_HPP
