// Andrew Naplavkov

#ifndef BRIG_TILING_DEF_HPP
#define BRIG_TILING_DEF_HPP

#include <brig/column_def.hpp>
#include <brig/identifier.hpp>
#include <vector>

namespace brig {

struct tiling_def {
  double resolution_x, resolution_y;
  identifier geometry;
  column_def raster;

  std::vector<column_def> query_conditions;

  tiling_def() : resolution_x(0), resolution_y(0)  {}
  double pixel_area() const  { return resolution_x * resolution_y; }
}; // tiling_def

} // brig

#endif // BRIG_TILING_DEF_HPP
