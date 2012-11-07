// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_FIT_RASTER_HPP
#define BRIG_DATABASE_DETAIL_FIT_RASTER_HPP

#include <algorithm>
#include <brig/database/command.hpp>
#include <brig/database/detail/dialect.hpp>
#include <brig/database/raster_pyramid.hpp>
#include <iterator>
#include <string>

namespace brig { namespace database { namespace detail {

inline raster_pyramid fit_raster(dialect* dct, const raster_pyramid& raster, const std::string& schema)
{
  using namespace std;

  raster_pyramid res;
  for (auto lvl_iter(begin(raster.levels)); lvl_iter != end(raster.levels); ++lvl_iter)
  {
    raster_level lvl;
    lvl.resolution_x = lvl_iter->resolution_x;
    lvl.resolution_y = lvl_iter->resolution_y;
    lvl.geometry.schema = schema;
    lvl.geometry.name = dct->fit_identifier(lvl_iter->geometry.name);
    lvl.geometry.qualifier = dct->fit_identifier(lvl_iter->geometry.qualifier);
    lvl.raster = dct->fit_column(lvl_iter->raster);
    res.levels.push_back(lvl);
  }
  sort(begin(res.levels), end(res.levels), [](const raster_level& a, const raster_level& b){ return a.resolution_x * a.resolution_y < b.resolution_x * b.resolution_y; });
  res.id.schema = res.levels.front().geometry.schema;
  res.id.name = res.levels.front().geometry.name;
  res.id.qualifier = res.levels.front().raster.name;
  return res;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_FIT_RASTER_HPP
