// Andrew Naplavkov

#ifndef BRIG_RASTER_PYRAMID_HPP
#define BRIG_RASTER_PYRAMID_HPP

#include <brig/raster_level.hpp>
#include <brig/identifier.hpp>
#include <vector>

namespace brig {

struct raster_pyramid {
  identifier id;
  std::vector<raster_level> levels;
}; // raster_pyramid

} // brig

#endif // BRIG_RASTER_PYRAMID_HPP
