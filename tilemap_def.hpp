// Andrew Naplavkov

#ifndef BRIG_TILEMAP_DEF_HPP
#define BRIG_TILEMAP_DEF_HPP

#include <brig/column_def.hpp>
#include <brig/identifier.hpp>
#include <vector>

namespace brig {

struct tilemap_def {
  double resolution_x, resolution_y;
  identifier geometry;
  column_def raster;

  std::vector<column_def> query_conditions;

  tilemap_def() : resolution_x(0), resolution_y(0)  {}
  double pixel_area() const  { return resolution_x * resolution_y; }
}; // tilemap_def

} // brig

#endif // BRIG_TILEMAP_DEF_HPP
