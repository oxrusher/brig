// Andrew Naplavkov

#ifndef BRIG_WKT_SCAN_HPP
#define BRIG_WKT_SCAN_HPP

#include <brig/blob_t.hpp>
#include <brig/boost/as_binary.hpp>
#include <brig/boost/geom_from_text.hpp>

namespace brig { namespace wkt {

template <typename Text>
blob_t scan(Text wkt)
{
  using namespace brig::boost;
  return as_binary(geom_from_text(wkt));
}

} } // brig::wkt

#endif // BRIG_WKT_SCAN_HPP
