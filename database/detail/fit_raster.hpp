// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_FIT_RASTER_HPP
#define BRIG_DATABASE_DETAIL_FIT_RASTER_HPP

#include <algorithm>
#include <brig/database/command.hpp>
#include <brig/database/detail/dialect.hpp>
#include <brig/pyramid_def.hpp>
#include <iterator>
#include <locale>
#include <sstream>
#include <string>

namespace brig { namespace database { namespace detail {

inline pyramid_def fit_raster(dialect* dct, const pyramid_def& raster, const std::string& schema)
{
  using namespace std;
  pyramid_def res;
  res.id.schema = schema;
  res.id.name = dct->fit_identifier(raster.id.name);
  vector<tilemap_def> levels(raster.levels);
  sort(begin(levels), end(levels), [](const tilemap_def& a, const tilemap_def& b){ return a.get_pixel_area() < b.get_pixel_area(); });
  const int width(levels.size() < 10? 1: 2);
  for (size_t i(0); i < levels.size(); ++i)
  {
    tilemap_def lvl;
    lvl.resolution_x = levels[i].resolution_x;
    lvl.resolution_y = levels[i].resolution_y;
    lvl.geometry.schema = schema;
    lvl.geometry.qualifier = dct->fit_identifier(levels[i].geometry.qualifier);
    lvl.raster = dct->fit_column(levels[i].raster);
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
