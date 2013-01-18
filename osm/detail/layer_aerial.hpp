// Andrew Naplavkov

#ifndef BRIG_OSM_DETAIL_LAYER_AERIAL_HPP
#define BRIG_OSM_DETAIL_LAYER_AERIAL_HPP

#include <brig/osm/detail/layer.hpp>
#include <brig/string_cast.hpp>

namespace brig { namespace osm { namespace detail {

struct layer_aerial : layer {
  virtual int max_zoom()  { return 11; }
  std::string get_name() override  { return "Aerial"; }
  std::string get_url(size_t order, const tile& tl) override;
}; // layer_aerial

inline std::string layer_aerial::get_url(size_t order, const tile& tl)
{
  return "http://oatile" + string_cast<char>((order % 4) + 1) + ".mqcdn.com/naip/" + string_cast<char>(tl.z) + "/" + string_cast<char>(tl.x) + "/" + string_cast<char>(tl.y) + ".png";
}

} } } // brig::osm::detail

#endif // BRIG_OSM_DETAIL_LAYER_AERIAL_HPP
