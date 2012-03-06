// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_RASTERS_HPP
#define BRIG_DATABASE_DETAIL_GET_RASTERS_HPP

#include <brig/boost/geometry.hpp>
#include <brig/database/identifier.hpp>
#include <brig/database/numeric_cast.hpp>
#include <brig/database/raster_detail.hpp>
#include <brig/database/raster_pyramid.hpp>
#include <brig/database/rowset.hpp>
#include <brig/database/variant.hpp>
#include <brig/detail/string_cast.hpp>
#include <memory>
#include <vector>

namespace brig { namespace database { namespace detail {

inline std::vector<raster_pyramid<raster_detail>> get_rasters(std::shared_ptr<rowset> rs)
{
  using namespace brig::detail;
  std::vector<raster_pyramid<raster_detail>> res;
  std::vector<variant> row;
  identifier prev_base;
  while (rs->fetch(row))
  {
    identifier cur_base;
    cur_base.schema = string_cast<char>(row[0]);
    cur_base.name = string_cast<char>(row[1]);
    cur_base.qualifier = string_cast<char>(row[2]);
    if (cur_base.schema != prev_base.schema || cur_base.name != prev_base.name || cur_base.qualifier != prev_base.qualifier)
    {
      res.push_back(raster_pyramid<raster_detail>());
      res.back().base = cur_base;
      prev_base = cur_base;
    }

    raster_detail raster;
    double res_x(0), res_y(0);
    numeric_cast(row[3], res_x);
    numeric_cast(row[4], res_y);
    raster.resolution = brig::boost::point(res_x, res_y);
    raster.geometry_layer.schema = string_cast<char>(row[5]);
    raster.geometry_layer.name = string_cast<char>(row[6]);
    raster.geometry_layer.qualifier = string_cast<char>(row[7]);
    raster.raster_column = string_cast<char>(row[8]);
    res.back().push_back(raster);
  }
  return res;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_RASTERS_HPP
