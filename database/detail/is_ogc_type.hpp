// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_IS_OGC_TYPE_HPP
#define BRIG_DATABASE_DETAIL_IS_OGC_TYPE_HPP

#include <algorithm>
#include <iterator>
#include <string>

namespace brig { namespace database { namespace detail {

inline bool is_ogc_type(const std::string& lower_case_type)
{
  static const char* types[] =
  { "collection", "st_collection" // synonym
  , "curve", "st_curve" // non-instantiable
  , "geomcollection", "st_geomcollection" // synonym
  , "geometry", "st_geometry" // non-instantiable
  , "geometrycollection", "st_geometrycollection"
  , "linestring", "st_linestring"
  , "multicurve", "st_multicurve" // non-instantiable
  , "multilinestring", "st_multilinestring"
  , "multipoint", "st_multipoint"
  , "multipolygon", "st_multipolygon"
  , "multisurface", "st_multisurface" // non-instantiable
  , "point", "st_point"
  , "polygon", "st_polygon"
  , "surface", "st_surface" // non-instantiable
  };
  auto iter = std::find_if(std::begin(types), std::end(types), [&](const char* type){ return lower_case_type.compare(type) == 0;  });
  return iter != std::end(types) && lower_case_type.compare(*iter) == 0;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_IS_OGC_TYPE_HPP
