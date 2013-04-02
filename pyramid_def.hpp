// Andrew Naplavkov

#ifndef BRIG_PYRAMID_DEF_HPP
#define BRIG_PYRAMID_DEF_HPP

#include <algorithm>
#include <brig/tilemap_def.hpp>
#include <brig/identifier.hpp>
#include <cmath>
#include <iterator>
#include <vector>

namespace brig {

struct pyramid_def {
  identifier id;
  std::vector<tilemap_def> levels;

  size_t snap_to_level(double pixel_area) const;
}; // pyramid_def

inline size_t pyramid_def::snap_to_level(double pixel_area) const
{
  using namespace std;
  return distance(begin(levels), min_element(begin(levels), end(levels), [=](const tilemap_def& a, const tilemap_def& b)
  {
    return fabs(a.pixel_area() - pixel_area) < fabs(b.pixel_area() - pixel_area);
  }));
} // pyramid_def::

} // brig

#endif // BRIG_PYRAMID_DEF_HPP
