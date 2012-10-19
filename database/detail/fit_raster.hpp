// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_FIT_RASTER_HPP
#define BRIG_DATABASE_DETAIL_FIT_RASTER_HPP

#include <algorithm>
#include <brig/database/detail/fit_column.hpp>
#include <brig/database/detail/fit_identifier.hpp>
#include <brig/database/global.hpp>
#include <brig/database/raster_pyramid.hpp>
#include <iterator>

namespace brig { namespace database { namespace detail {

inline raster_pyramid fit_raster(const raster_pyramid& raster_from, DBMS sys_to, const std::string& schema_to)
{
  using namespace std;

  raster_pyramid raster_to;

  for (auto lvl_from(begin(raster_from.levels)); lvl_from != end(raster_from.levels); ++lvl_from)
  {
    raster_level lvl_to;
    lvl_to.resolution = lvl_from->resolution;
    lvl_to.geometry = fit_identifier(lvl_from->geometry, sys_to, schema_to);
    lvl_to.raster = fit_column(lvl_from->raster, sys_to);
    raster_to.levels.push_back(lvl_to);
  }

  sort(begin(raster_to.levels), end(raster_to.levels), [](const raster_level& a, const raster_level& b){ return a.resolution.get<0>() * a.resolution.get<1>() < b.resolution.get<0>() * b.resolution.get<1>(); });

  raster_to.id.schema = raster_to.levels.front().geometry.schema;
  raster_to.id.name = raster_to.levels.front().geometry.name;
  raster_to.id.qualifier = raster_to.levels.front().raster.name;
  return raster_to;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_FIT_RASTER_HPP
