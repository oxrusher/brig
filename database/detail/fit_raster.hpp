// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_FIT_RASTER_HPP
#define BRIG_DATABASE_DETAIL_FIT_RASTER_HPP

#include <brig/database/command.hpp>
#include <brig/database/detail/dialect.hpp>
#include <brig/database/raster_pyramid.hpp>
#include <brig/string_cast.hpp>
#include <string>

namespace brig { namespace database { namespace detail {

inline raster_pyramid fit_raster(dialect* dct, const raster_pyramid& raster, const std::string& schema)
{
  using namespace std;

  raster_pyramid res;
  res.id.schema = schema;
  res.id.name = dct->fit_identifier(raster.id.name);
  res.id.qualifier = dct->fit_identifier(raster.id.qualifier);
  string lvl_0;
  for (size_t i(0); i < raster.levels.size(); ++i)
  {
    raster_level lvl;
    lvl.resolution_x = raster.levels[i].resolution_x;
    lvl.resolution_y = raster.levels[i].resolution_y;
    lvl.geometry.schema = schema;

    if (i == 0)
    {
      lvl.geometry.name = raster.levels[i].geometry.name;
      lvl_0 = lvl.geometry.name;
    }
    else
      lvl.geometry.name = lvl_0 + "_" + string_cast<char>(i);
    lvl.geometry.name = dct->fit_identifier(lvl.geometry.name);
   
    lvl.geometry.qualifier = dct->fit_identifier(raster.levels[i].geometry.qualifier);
    lvl.raster = dct->fit_column(raster.levels[i].raster);
    res.levels.push_back(lvl);
  }
  return res;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_FIT_RASTER_HPP
