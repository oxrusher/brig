// Andrew Naplavkov

#ifndef BRIG_OSM_LAYER_HPP
#define BRIG_OSM_LAYER_HPP

#include <string>

namespace brig { namespace osm {

struct layer {
  virtual ~layer()  {}
  virtual int get_pixels()  { return 256; }
  virtual int get_max_zoom()  { return 18; }
  virtual std::string get_name() = 0;
  virtual std::string get_url(size_t order, int x, int y, int z) = 0;
}; // layer

} } // brig::osm

#endif // BRIG_OSM_LAYER_HPP
