// Andrew Naplavkov

#ifndef BRIG_RASTER_LEVEL_HPP
#define BRIG_RASTER_LEVEL_HPP

#include <brig/column_definition.hpp>
#include <brig/identifier.hpp>
#include <vector>

namespace brig {

struct raster_level {
  double resolution_x, resolution_y;
  identifier geometry;
  column_definition raster;

  std::vector<column_definition> query_conditions;

  raster_level() : resolution_x(0), resolution_y(0)  {}
}; // raster_level

} // brig

#endif // BRIG_RASTER_LEVEL_HPP
