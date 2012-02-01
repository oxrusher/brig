// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_IS_OGC_TYPE_HPP
#define BRIG_DATABASE_DETAIL_IS_OGC_TYPE_HPP

#include <boost/algorithm/string.hpp>
#include <locale>
#include <string>

namespace brig { namespace database { namespace detail {

inline bool is_ogc_type(const std::string& type)
{
  using namespace boost::algorithm;
  auto loc = std::locale::classic();
  static const char* Types[] = {
      "COLLECTION" // synonym
    , "CURVE" // non-instantiable
    , "GEOMCOLLECTION" // synonym
    , "GEOMETRY" // non-instantiable
    , "GEOMETRYCOLLECTION"
    , "LINESTRING"
    , "MULTICURVE" // non-instantiable
    , "MULTILINESTRING"
    , "MULTIPOINT"
    , "MULTIPOLYGON"
    , "MULTISURFACE" // non-instantiable
    , "POINT"
    , "POLYGON"
    , "SURFACE" // non-instantiable
  };
  const std::string prefix("ST_");
  for (size_t i(0); i < sizeof(Types) / sizeof(Types[0]); ++i)
    if (iequals(type, Types[i], loc) || iequals(type, prefix + Types[i], loc)) return true;
  return false;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_IS_OGC_TYPE_HPP
