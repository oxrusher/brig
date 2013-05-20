// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_IS_OGC_TYPE_HPP
#define BRIG_DATABASE_DETAIL_IS_OGC_TYPE_HPP

#include <algorithm>
#include <iterator>
#include <string>

namespace brig { namespace database { namespace detail {

inline bool is_ogc_type(const std::string& type_lcase)
{
  using namespace std;

  static const char* types[] =
  { "collection" // synonym
  , "curve" // non-instantiable
  , "geomcollection" // synonym
  , "geometry" // non-instantiable
  , "geometrycollection"
  , "linestring"
  , "multicurve" // non-instantiable
  , "multilinestring"
  , "multipoint"
  , "multipolygon"
  , "multisurface" // non-instantiable
  , "point"
  , "polygon"
  , "surface" // non-instantiable
  };
  const string prefix("st_");
  auto itr(find_if(begin(types), end(types), [&](const char* type){ return type_lcase.compare(type) == 0 || type_lcase.compare(prefix + type) == 0; }));
  return itr != end(types);
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_IS_OGC_TYPE_HPP
