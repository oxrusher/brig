// Andrew Naplavkov

#ifndef BRIG_OSM_DETAIL_LAYER_MAPQUEST_HPP
#define BRIG_OSM_DETAIL_LAYER_MAPQUEST_HPP

#include <brig/osm/detail/layer.hpp>
#include <brig/string_cast.hpp>

namespace brig { namespace osm { namespace detail {

struct layer_mapquest : layer {
  std::string get_name() override  { return "MapQuest"; }
  std::string get_url(size_t order, const tile& tl) override;
}; // layer_mapquest

inline std::string layer_mapquest::get_url(size_t order, const tile& tl)
{
  return "http://otile" + string_cast<char>((order % 4) + 1) + ".mqcdn.com/tiles/1.0.0/osm/" + string_cast<char>(tl.z) + "/" + string_cast<char>(tl.x) + "/" + string_cast<char>(tl.y) + ".png";
}

} } } // brig::osm::detail

#endif // BRIG_OSM_DETAIL_LAYER_MAPQUEST_HPP
