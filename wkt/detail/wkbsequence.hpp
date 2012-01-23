// Andrew Naplavkov

#ifndef BRIG_WKT_DETAIL_WKBSEQUENCE_HPP
#define BRIG_WKT_DETAIL_WKBSEQUENCE_HPP

#include <brig/detail/ogc.hpp>
#include <brig/wkt/detail/linearring.hpp>
#include <brig/wkt/detail/wkbpoint.hpp>
#include <cstdint>
#include <vector>

namespace brig { namespace wkt { namespace detail {

template <typename T, uint32_t GeometryType>
struct wkbsequence
{
  static const uint32_t geometry_type = GeometryType;
  std::vector<T> elements;
};

using namespace brig::detail;

struct wkblinestring : wkbsequence<point_t, ogc::LineString>
{
  wkblinestring()  {}
  wkblinestring(const linearring& line_)  { elements = line_.points; }
};

typedef wkbsequence<linearring, ogc::Polygon> wkbpolygon;
typedef wkbsequence<wkbpoint, ogc::MultiPoint> wkbmultipoint;
typedef wkbsequence<wkblinestring, ogc::MultiLineString> wkbmultilinestring;
typedef wkbsequence<wkbpolygon, ogc::MultiPolygon> wkbmultipolygon;

template <typename OutputIterator, typename WKBSequence>
void set(OutputIterator& out_iter, const WKBSequence& seq)
{
  ogc::set_byte_order(out_iter);
  ogc::set<uint32_t>(out_iter, seq.geometry_type);
  ogc::set<uint32_t>(out_iter, static_cast<uint32_t>(seq.elements.size()));
  for (size_t i(0); i < seq.elements.size(); ++i)
    set<>(out_iter, seq.elements[i]);
}

} } } // brig::wkt::detail

#endif // BRIG_WKT_DETAIL_WKBSEQUENCE_HPP
