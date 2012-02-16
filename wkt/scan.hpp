// Andrew Naplavkov

#ifndef BRIG_WKT_SCAN_HPP
#define BRIG_WKT_SCAN_HPP

#include <brig/blob_t.hpp>
#include <brig/boost/geometry.hpp>
#include <brig/boost/read_wkt.hpp>
#include <brig/boost/write_wkb.hpp>
#include <string>

namespace brig { namespace wkt {

inline void scan(const char* in_wkt, blob_t& out_wkb)
{
  using namespace brig::boost;
  geometry geom;
  read_wkt(in_wkt, geom);
  write_wkb(geom, out_wkb);
}

inline void scan(const std::string& in_wkt, blob_t& out_wkb)
{
  using namespace brig::boost;
  geometry geom;
  read_wkt(in_wkt, geom);
  write_wkb(geom, out_wkb);
}

} } // brig::wkt

#endif // BRIG_WKT_SCAN_HPP
