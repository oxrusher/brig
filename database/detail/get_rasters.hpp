// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_RASTERS_HPP
#define BRIG_DATABASE_DETAIL_GET_RASTERS_HPP

#include <brig/boost/geometry.hpp>
#include <brig/database/identifier.hpp>
#include <brig/database/numeric_cast.hpp>
#include <brig/database/raster_level.hpp>
#include <brig/database/raster_pyramid.hpp>
#include <brig/database/rowset.hpp>
#include <brig/database/variant.hpp>
#include <brig/string_cast.hpp>
#include <memory>
#include <vector>

namespace brig { namespace database { namespace detail {

inline std::vector<raster_pyramid> get_rasters(std::shared_ptr<rowset> rs)
{
  std::vector<raster_pyramid> res;
  std::vector<variant> row;
  identifier prev_id;
  while (rs->fetch(row))
  {
    identifier cur_id;
    cur_id.schema = string_cast<char>(row[0]);
    cur_id.name = string_cast<char>(row[1]);
    cur_id.qualifier = string_cast<char>(row[2]);
    if (cur_id.schema != prev_id.schema || cur_id.name != prev_id.name || cur_id.qualifier != prev_id.qualifier)
    {
      res.push_back(raster_pyramid());
      res.back().schema = cur_id.schema;
      res.back().name = cur_id.name;
      res.back().qualifier = cur_id.qualifier;
      prev_id = cur_id;
    }

    raster_level lvl;
    double res_x(0), res_y(0);
    numeric_cast(row[3], res_x);
    numeric_cast(row[4], res_y);
    lvl.resolution = brig::boost::point(res_x, res_y);
    lvl.geometry.schema = string_cast<char>(row[5]);
    lvl.geometry.name = string_cast<char>(row[6]);
    lvl.geometry.qualifier = string_cast<char>(row[7]);
    lvl.raster.name = string_cast<char>(row[8]);
    res.back().levels.push_back(lvl);
  }
  return res;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_RASTERS_HPP
