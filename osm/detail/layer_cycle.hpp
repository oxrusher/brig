// Andrew Naplavkov

#ifndef BRIG_OSM_DETAIL_LAYER_CYCLE_HPP
#define BRIG_OSM_DETAIL_LAYER_CYCLE_HPP

#include <brig/osm/detail/layer.hpp>
#include <brig/string_cast.hpp>

namespace brig { namespace osm { namespace detail {

struct layer_cycle : layer {
  std::string get_name() override  { return "OpenCycleMap"; }
  std::string get_url(size_t order, const tile& tl) override;
}; // layer_cycle

inline std::string layer_cycle::get_url(size_t order, const tile& tl)
{
  std::string subdomain;
  switch (order % 3)
  {
  case 0: subdomain = "a"; break;
  case 1: subdomain = "b"; break;
  case 2: subdomain = "c"; break;
  }
  return "http://" + subdomain + ".tile.opencyclemap.org/cycle/" + string_cast<char>(tl.z) + "/" + string_cast<char>(tl.x) + "/" + string_cast<char>(tl.y) + ".png";
}

} } } // brig::osm::detail

#endif // BRIG_OSM_DETAIL_LAYER_CYCLE_HPP
