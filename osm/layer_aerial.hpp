// Andrew Naplavkov

#ifndef BRIG_OSM_LAYER_AERIAL_HPP
#define BRIG_OSM_LAYER_AERIAL_HPP

#include <brig/osm/layer.hpp>
#include <brig/string_cast.hpp>

namespace brig { namespace osm {

struct layer_aerial : layer {
  int get_max_zoom() override  { return 11; }
  std::string get_name() override  { return "Aerial"; }
  std::string get_url(size_t order, int x, int y, int z) override
  {
    return "http://oatile" + string_cast<char>((order % 4) + 1) + ".mqcdn.com/naip/" + string_cast<char>(z) + "/" + string_cast<char>(x) + "/" + string_cast<char>(y) + ".png";
  }
}; // layer_aerial

} } // brig::osm

#endif // BRIG_OSM_LAYER_AERIAL_HPP
