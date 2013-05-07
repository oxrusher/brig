// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_NORMALIZE_HEMISPHERE_HPP
#define BRIG_DATABASE_DETAIL_NORMALIZE_HEMISPHERE_HPP

#include <brig/boost/geometry.hpp>
#include <stdexcept>
#include <vector>

namespace brig { namespace database { namespace detail {

inline void normalize_hemisphere(std::vector<boost::box>& boxes)
{
  using namespace brig::boost;

  size_t i(0);
  while (i < boxes.size())
  {
    const double xdif(boxes[i].max_corner().get<0>() - boxes[i].min_corner().get<0>());
    const double ydif(boxes[i].max_corner().get<1>() - boxes[i].min_corner().get<1>());
    if (0 > xdif || 0 > ydif) throw std::runtime_error("box error");
    if (120 < xdif)
    {
      const double xmed(boxes[i].min_corner().get<0>() + xdif / 2.);
      boxes.push_back(box(point(boxes[i].min_corner().get<0>(), boxes[i].min_corner().get<1>()), point(xmed, boxes[i].max_corner().get<1>())));
      boxes[i] = box(point(xmed, boxes[i].min_corner().get<1>()), point(boxes[i].max_corner().get<0>(), boxes[i].max_corner().get<1>()));
    }
    else if (60 < ydif)
    {
      const double ymed(boxes[i].min_corner().get<1>() + ydif / 2.);
      boxes.push_back(box(point(boxes[i].min_corner().get<0>(), boxes[i].min_corner().get<1>()), point(boxes[i].max_corner().get<0>(), ymed)));
      boxes[i] = box(point(boxes[i].min_corner().get<0>(), ymed), point(boxes[i].max_corner().get<0>(), boxes[i].max_corner().get<1>()));
    }
    else
      ++i;
  }
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_NORMALIZE_HEMISPHERE_HPP
