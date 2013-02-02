// Andrew Naplavkov

#ifndef BRIG_OSM_LAYER_MAPQUEST_HPP
#define BRIG_OSM_LAYER_MAPQUEST_HPP

#include <brig/osm/layer.hpp>
#include <brig/string_cast.hpp>

namespace brig { namespace osm {

struct layer_mapquest : layer {
  std::string get_name() override  { return "MapQuest"; }
  std::string get_url(size_t order, int x, int y, int z) override
  {
    return "http://otile" + string_cast<char>((order % 4) + 1) + ".mqcdn.com/tiles/1.0.0/osm/" + string_cast<char>(z) + "/" + string_cast<char>(x) + "/" + string_cast<char>(y) + ".png";
  }
}; // layer_mapquest

} } // brig::osm

#endif // BRIG_OSM_LAYER_MAPQUEST_HPP
