// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_IS_OGC_TYPE_HPP
#define BRIG_DATABASE_DETAIL_IS_OGC_TYPE_HPP

#include <boost/algorithm/string.hpp>
#include <string>

namespace brig { namespace database { namespace detail {

inline bool is_ogc_type(std::string type)
{
  if (boost::algorithm::starts_with(type, "ST_")) boost::algorithm::replace_first(type, "ST_", "");
  return type == "COLLECTION" // synonym
      || type == "CURVE" // non-instantiable
      || type == "GEOMCOLLECTION" // synonym
      || type == "GEOMETRY" // non-instantiable
      || type == "GEOMETRYCOLLECTION"
      || type == "LINESTRING"
      || type == "MULTICURVE" // non-instantiable
      || type == "MULTILINESTRING"
      || type == "MULTIPOINT"
      || type == "MULTIPOLYGON"
      || type == "MULTISURFACE" // non-instantiable
      || type == "POINT"
      || type == "POLYGON"
      || type == "SURFACE" // non-instantiable
      ;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_IS_OGC_TYPE_HPP
