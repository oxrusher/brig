// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_FIT_RASTER_HPP
#define BRIG_DATABASE_DETAIL_FIT_RASTER_HPP

#include <brig/database/command.hpp>
#include <brig/database/detail/dialect.hpp>
#include <brig/raster_pyramid.hpp>
#include <locale>
#include <sstream>
#include <string>

namespace brig { namespace database { namespace detail {

inline raster_pyramid fit_raster(dialect* dct, const raster_pyramid& raster, const std::string& schema)
{
  using namespace std;
  raster_pyramid res;
  res.id.schema = schema;
  res.id.name = dct->fit_identifier(raster.id.name);
  const int width(raster.levels.size() < 10? 1: 2);
  for (size_t i(0); i < raster.levels.size(); ++i)
  {
    raster_level lvl;
    lvl.resolution_x = raster.levels[i].resolution_x;
    lvl.resolution_y = raster.levels[i].resolution_y;
    lvl.geometry.schema = schema;
    lvl.geometry.qualifier = dct->fit_identifier(raster.levels[i].geometry.qualifier);
    lvl.raster = dct->fit_column(raster.levels[i].raster);
    if (i == 0)
    {
      lvl.geometry.name = res.id.name;
      res.id.qualifier = lvl.raster.name;
    }
    else
    {
      ostringstream stream; stream.imbue(locale::classic());
      stream << res.id.name << "_l" << setfill('0') << setw(width) << i;
      lvl.geometry.name = dct->fit_identifier(stream.str());
    }
    res.levels.push_back(lvl);
  }
  return res;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_FIT_RASTER_HPP
