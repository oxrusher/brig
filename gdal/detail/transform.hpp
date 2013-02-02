// Andrew Naplavkov

#ifndef BRIG_GDAL_DETAIL_TRANSFORM_HPP
#define BRIG_GDAL_DETAIL_TRANSFORM_HPP

#include <boost/geometry/geometry.hpp>
#include <brig/boost/geometry.hpp>

namespace brig { namespace gdal { namespace detail {

class transform {
  double to_x(double p, double l) const  { return coef[0] + p*coef[1] + l*coef[2]; }
  double to_y(double p, double l) const  { return coef[3] + p*coef[4] + l*coef[5]; }
  double divisor() const  { return coef[1]*coef[5] - coef[2]*coef[4]; }
  double to_p(double x, double y) const  { return (coef[5]*(x - coef[0]) - coef[2]*(y - coef[3])) / divisor(); }
  double to_l(double x, double y) const  { return (coef[1]*(y - coef[3]) - coef[4]*(x - coef[0])) / divisor(); }

public:
  double coef[6];

  boost::polygon pixel_to_proj(const boost::box& env) const;
  boost::polygon proj_to_pixel(const boost::box& env) const;
}; // transform

inline boost::polygon transform::pixel_to_proj(const boost::box& env) const
{
  using namespace std;
  using namespace brig::boost;
  const double pmin(env.min_corner().get<0>()), lmin(env.min_corner().get<1>()), pmax(env.max_corner().get<0>()), lmax(env.max_corner().get<1>());
  polygon poly;
  ::boost::geometry::append(poly, point(to_x(pmin, lmax), to_y(pmin, lmax)));
  ::boost::geometry::append(poly, point(to_x(pmax, lmax), to_y(pmax, lmax)));
  ::boost::geometry::append(poly, point(to_x(pmax, lmin), to_y(pmax, lmin)));
  ::boost::geometry::append(poly, point(to_x(pmin, lmin), to_y(pmin, lmin)));
  ::boost::geometry::append(poly, point(to_x(pmin, lmax), to_y(pmin, lmax)));
  return poly;
}

inline boost::polygon transform::proj_to_pixel(const boost::box& env) const
{
  using namespace std;
  using namespace brig::boost;
  const double xmin(env.min_corner().get<0>()), ymin(env.min_corner().get<1>()), xmax(env.max_corner().get<0>()), ymax(env.max_corner().get<1>());
  polygon poly;
  ::boost::geometry::append(poly, point(to_p(xmin, ymax), to_l(xmin, ymax)));
  ::boost::geometry::append(poly, point(to_p(xmin, ymin), to_l(xmin, ymin)));
  ::boost::geometry::append(poly, point(to_p(xmax, ymin), to_l(xmax, ymin)));
  ::boost::geometry::append(poly, point(to_p(xmax, ymax), to_l(xmax, ymax)));
  ::boost::geometry::append(poly, point(to_p(xmin, ymax), to_l(xmin, ymax)));
  return poly;
}

} } } // brig::gdal::detail

#endif // BRIG_GDAL_DETAIL_TRANSFORM_HPP
