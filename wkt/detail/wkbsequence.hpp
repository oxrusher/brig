// Andrew Naplavkov

#ifndef BRIG_WKT_DETAIL_WKBSEQUENCE_HPP
#define BRIG_WKT_DETAIL_WKBSEQUENCE_HPP

#include <brig/detail/ogc.hpp>
#include <brig/wkt/detail/line_t.hpp>
#include <brig/wkt/detail/wkblinestring.hpp>
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

typedef wkbsequence<line_t, brig::detail::ogc::Polygon> wkbpolygon;
typedef wkbsequence<wkbpoint, brig::detail::ogc::MultiPoint> wkbmultipoint;
typedef wkbsequence<wkblinestring, brig::detail::ogc::MultiLineString> wkbmultilinestring;
typedef wkbsequence<wkbpolygon, brig::detail::ogc::MultiPolygon> wkbmultipolygon;

template <typename OutputIterator, typename WKBSequence>
void set(OutputIterator& out_iter, const WKBSequence& seq)
{
  brig::detail::ogc::set_byte_order(out_iter);
  brig::detail::ogc::set<uint32_t>(out_iter, seq.geometry_type);
  brig::detail::ogc::set<uint32_t>(out_iter, static_cast<uint32_t>(seq.elements.size()));
  for (size_t i(0); i < seq.elements.size(); ++i)
    set<>(out_iter, seq.elements[i]);
}

} } } // brig::wkt::detail

#endif // BRIG_WKT_DETAIL_WKBSEQUENCE_HPP
