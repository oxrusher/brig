// Andrew Naplavkov

#ifndef BRIG_OSM_PROVIDER_HPP
#define BRIG_OSM_PROVIDER_HPP

#include <brig/boost/envelope.hpp>
#include <brig/boost/geometry.hpp>
#include <brig/boost/geom_from_wkb.hpp>
#include <brig/detail/get_columns.hpp>
#include <brig/global.hpp>
#include <brig/numeric_cast.hpp>
#include <brig/osm/detail/rowset.hpp>
#include <brig/osm/detail/rowset_lite.hpp>
#include <brig/osm/detail/tile.hpp>
#include <brig/osm/layer.hpp>
#include <brig/provider.hpp>
#include <brig/variant.hpp>
#include <cstdlib>
#include <iterator>
#include <locale>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace brig { namespace osm {

class provider : public brig::provider {
  std::shared_ptr<layer> m_lr;

  int table_to_zoom(const std::string& tbl);
  std::string zoom_to_table(int z);

public:
  explicit provider(std::shared_ptr<layer> lr) : m_lr(lr)  {}

  std::vector<identifier> get_tables() override;
  std::vector<identifier> get_geometry_layers() override;
  std::vector<pyramid_def> get_raster_layers() override;
  table_def get_table_def(const identifier& tbl) override;
  boost::box get_extent(const table_def& tbl) override;
  std::shared_ptr<rowset> select(const table_def& tbl) override;

  bool is_readonly() override  { return true; }
  table_def fit_to_create(const table_def&) override  { throw std::runtime_error("OSM error"); }
  void create(const table_def&) override  { throw std::runtime_error("OSM error"); }
  void drop(const table_def&) override  { throw std::runtime_error("OSM error"); }
  pyramid_def fit_to_reg(const pyramid_def&) override  { throw std::runtime_error("OSM error"); }
  void reg(const pyramid_def&) override  { throw std::runtime_error("OSM error"); }
  void unreg(const pyramid_def&) override  { throw std::runtime_error("OSM error"); }
  std::shared_ptr<inserter> get_inserter(const table_def&) override  { throw std::runtime_error("OSM error"); }
}; // provider

inline int provider::table_to_zoom(const std::string& tbl)
{
  using namespace std;
  const string name(TableName);
  if (tbl.size() < name.size() || tbl.substr(0, name.size()).compare(name) != 0) throw runtime_error("OSM error");
  if (tbl.size() == name.size()) return m_lr->get_max_zoom();
  variant v = tbl.substr(name.size() + 6);
  int res(-1);
  if (!numeric_cast(v, res) || res < 0 || res > m_lr->get_max_zoom()) throw runtime_error("OSM error");
  return res;
}

inline std::string provider::zoom_to_table(int z)
{
  using namespace std;
  ostringstream stream; stream.imbue(locale::classic());
  stream << TableName;
  if (m_lr->get_max_zoom() != z)
  {
    stream << "_l" << setfill('0') << setw(2) << (m_lr->get_max_zoom() - z);
    stream << "_z" << setfill('0') << setw(2) << z;
  }
  return stream.str();
}

inline std::vector<identifier> provider::get_tables()
{
  using namespace detail;
  std::vector<identifier> res;
  for (int z(m_lr->get_max_zoom()); z >= 0; --z)
  {
    identifier id;
    id.name = zoom_to_table(z);
    res.push_back(id);
  }
  return res;
}

inline std::vector<identifier> provider::get_geometry_layers()
{
  using namespace detail;
  std::vector<identifier> res;
  for (int z(m_lr->get_max_zoom()); z >= 0; --z)
  {
    identifier id;
    id.name = zoom_to_table(z);
    id.qualifier = ColumnNameWkb;
    res.push_back(id);
  }
  return res;
}

inline std::vector<pyramid_def> provider::get_raster_layers()
{
  using namespace detail;
  std::vector<pyramid_def> res;

  pyramid_def pyr;
  pyr.id.name = zoom_to_table(m_lr->get_max_zoom());
  pyr.id.qualifier = ColumnNamePng;
  for (int z(m_lr->get_max_zoom()); z >= 0; --z)
  {
    auto env(tile(0, 0, z).get_box());
    tilemap_def lvl;
    lvl.resolution_x = (env.max_corner().get<0>() - env.min_corner().get<0>()) / double(m_lr->get_pixels());
    lvl.resolution_y = (env.max_corner().get<1>() - env.min_corner().get<1>()) / double(m_lr->get_pixels());
    lvl.geometry.name = zoom_to_table(z);
    lvl.geometry.qualifier = ColumnNameWkb;
    lvl.raster.name = ColumnNamePng;
    lvl.raster.type = column_type::Blob;
    pyr.levels.push_back(lvl);
  }
  res.push_back(pyr);

  return res;
}

inline table_def provider::get_table_def(const identifier& tbl)
{
  using namespace detail;

  table_def res;
  res.id = tbl;

  {
    column_def col;
    col.name = ColumnNameWkb;
    col.type = column_type::Geometry;
    col.epsg = 3395; // http://spatialreference.org/ref/epsg/3395/
    res.columns.push_back(col);
  }

  {
    column_def col;
    col.name = ColumnNamePng;
    col.type = column_type::Blob;
    res.columns.push_back(col);
  }

  index_def idx;
  idx.type = index_type::Spatial;
  idx.columns.push_back(ColumnNameWkb);
  res.indexes.push_back(idx);

  return res;
}

inline boost::box provider::get_extent(const table_def&)
{
  return detail::tile(0, 0, 0).get_box();
}

inline std::shared_ptr<rowset> provider::select(const table_def& tbl)
{
  using namespace std;
  using namespace brig::boost;
  using namespace detail;

  const int z(table_to_zoom(tbl.id.name));
  if (typeid(null_t) != tbl[ColumnNamePng]->query_value.type()) throw runtime_error("OSM error");

  vector<column_def> col_defs = tbl.query_columns.empty()? tbl.columns: brig::detail::get_columns(tbl.columns, tbl.query_columns);
  vector<bool> cols;
  bool lite(true);
  for (auto iter(begin(col_defs)); iter != end(col_defs); ++iter)
  {
    if (iter->name.compare(ColumnNamePng) == 0)
    {
      cols.push_back(true);
      lite = false;
    }
    else if (iter->name.compare(ColumnNameWkb) == 0)
      cols.push_back(false);
    else
      throw runtime_error("OSM error");
  }

  auto geom_col(tbl[ColumnNameWkb]);
  auto env((typeid(blob_t) == geom_col->query_value.type())? envelope(geom_from_wkb(::boost::get<blob_t>(geom_col->query_value))): tile(0, 0, 0).get_box());
  if (lite)
    return make_shared<rowset_lite>(cols.size(), z, env, tbl.query_rows);
  else
    return make_shared<rowset>(m_lr, cols, z, env, tbl.query_rows);
} // provider::

} } // brig::osm

#endif // BRIG_OSM_PROVIDER_HPP
