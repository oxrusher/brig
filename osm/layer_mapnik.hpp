// Andrew Naplavkov

#ifndef BRIG_OSM_LAYER_MAPNIK_HPP
#define BRIG_OSM_LAYER_MAPNIK_HPP

#include <brig/osm/layer.hpp>
#include <brig/string_cast.hpp>

namespace brig { namespace osm {

struct layer_mapnik : layer {
  std::string get_name() override  { return "Mapnik"; }
  std::string get_url(size_t order, int x, int y, int z) override
  {
    std::string subdomain;
    switch (order % 3)
    {
    case 0: subdomain = "a"; break;
    case 1: subdomain = "b"; break;
    case 2: subdomain = "c"; break;
    }
    return "http://" + subdomain + ".tile.openstreetmap.org/" + string_cast<char>(z) + "/" + string_cast<char>(x) + "/" + string_cast<char>(y) + ".png";
  }
}; // layer_mapnik

} } // brig::osm

#endif // BRIG_OSM_LAYER_MAPNIK_HPP
