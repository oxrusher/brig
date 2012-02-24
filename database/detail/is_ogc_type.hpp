// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_IS_OGC_TYPE_HPP
#define BRIG_DATABASE_DETAIL_IS_OGC_TYPE_HPP

#include <algorithm>
#include <string>

namespace brig { namespace database { namespace detail {

inline bool is_ogc_type(const std::string& case_folded_type)
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

  auto begin = types;
  auto end = types + sizeof(types) / sizeof(types[0]);
  auto iter = std::find_if(begin, end, [&](const char* type){ return case_folded_type.compare(type) == 0;  });
  return iter != end && case_folded_type.compare(*iter) == 0;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_IS_OGC_TYPE_HPP
