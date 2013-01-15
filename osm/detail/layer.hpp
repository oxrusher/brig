// Andrew Naplavkov

#ifndef BRIG_OSM_DETAIL_LAYER_HPP
#define BRIG_OSM_DETAIL_LAYER_HPP

#include <brig/numeric_cast.hpp>
#include <brig/osm/detail/tile.hpp>
#include <brig/variant.hpp>
#include <locale>
#include <sstream>

namespace brig { namespace osm { namespace detail {

struct layer {
  static const int MaxZoom = 18;
  static const int Pixels = 256;
  static std::string column_geometry()  { return "Geometry"; }
  static std::string column_raster()  { return "Raster"; }

  virtual ~layer()  {}
  virtual std::string get_name() = 0;
  virtual std::string get_url(size_t order, const tile& tl) = 0;

  std::string zoom_to_table(int z);
  int table_to_zoom(const std::string& tbl);
}; // layer

inline std::string layer::zoom_to_table(int z)
{
  using namespace std;
  ostringstream stream; stream.imbue(locale::classic());
  stream << get_name() << "_" << setfill('0') << setw(2) << z;
  return stream.str();
}

inline int layer::table_to_zoom(const std::string& tbl)
{
  using namespace std;
  const string name(get_name());
  if (tbl.size() < name.size() || tbl.substr(0, name.size()).compare(name) != 0) return -1;
  variant v = tbl.substr(name.size() + 1);
  int res(-1);
  return (numeric_cast(v, res) && res >= 0 && res <= MaxZoom)? res: -1;
} // layer::

} } } // brig::osm::detail

#endif // BRIG_OSM_DETAIL_LAYER_HPP
