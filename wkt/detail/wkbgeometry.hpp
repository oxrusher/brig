// Andrew Naplavkov

#ifndef BRIG_WKT_DETAIL_WKBGEOMETRY_HPP
#define BRIG_WKT_DETAIL_WKBGEOMETRY_HPP

#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <boost/variant/variant.hpp>
#include <brig/detail/ogc.hpp>
#include <brig/wkt/detail/wkbpoint.hpp>
#include <brig/wkt/detail/wkbsequence.hpp>
#include <vector>

namespace brig { namespace wkt { namespace detail {

struct wkbgeometrycollection;

typedef boost::variant<
  wkbpoint,
  wkblinestring,
  wkbpolygon,
  wkbmultipoint,
  wkbmultilinestring,
  wkbmultipolygon,
  boost::recursive_wrapper<wkbgeometrycollection>
> wkbvariant;

struct wkbgeometry  { wkbvariant var; };
struct wkbgeometrycollection  { std::vector<wkbgeometry> geoms; };

template <typename OutputIterator>
void set(OutputIterator& out_iter, const wkbgeometrycollection& collection);

template <typename OutputIterator>
struct set_visitor : boost::static_visitor<void> {
  OutputIterator& out_iter;
  explicit set_visitor(OutputIterator& out_iter_) : out_iter(out_iter_)  {}
  template <typename T>
  void operator()(const T& r) const  { set<>(out_iter, r); }
  void operator()(const boost::recursive_wrapper<wkbgeometrycollection>& r) const  { set<>(out_iter, r.get()); }
};

template <typename OutputIterator>
void set(OutputIterator& out_iter, const wkbgeometry& geom)
{
  boost::apply_visitor(set_visitor<OutputIterator>(out_iter), geom.var);
}

template <typename OutputIterator>
void set(OutputIterator& out_iter, const wkbgeometrycollection& collection)
{
  using namespace brig::detail;
  ogc::set_byte_order(out_iter);
  ogc::set<uint32_t>(out_iter, ogc::GeometryCollection);
  ogc::set<uint32_t>(out_iter, static_cast<uint32_t>(collection.geoms.size()));
  for (size_t i(0); i < collection.geoms.size(); ++i)
    set<>(out_iter, collection.geoms[i]);
};

} } } // brig::wkt::detail

#endif // BRIG_WKT_DETAIL_WKBGEOMETRY_HPP
