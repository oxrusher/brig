// Andrew Naplavkov

#ifndef BRIG_OSM_CONNECTION_HPP
#define BRIG_OSM_CONNECTION_HPP

#include <brig/boost/envelope.hpp>
#include <brig/boost/geometry.hpp>
#include <brig/boost/geom_from_wkb.hpp>
#include <brig/connection.hpp>
#include <brig/detail/get_columns.hpp>
#include <brig/global.hpp>
#include <brig/osm/detail/layer.hpp>
#include <brig/osm/detail/layer_aerial.hpp>
#include <brig/osm/detail/layer_cycle.hpp>
#include <brig/osm/detail/layer_mapquest.hpp>
#include <brig/osm/detail/layer_osm.hpp>
#include <brig/osm/detail/rowset.hpp>
#include <brig/osm/detail/rowset_lite.hpp>
#include <brig/osm/detail/tile.hpp>
#include <cstdlib>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <vector>

namespace brig { namespace osm {

class connection : public brig::connection {
  std::vector<std::shared_ptr<detail::layer>> m_lrs;

  void parse_table(const std::string& tbl, size_t& lr, int& z);

public:
  connection();

  std::vector<identifier> get_tables() override;
  std::vector<identifier> get_geometry_layers() override;
  std::vector<pyramid_def> get_raster_layers() override;
  table_def get_table_def(const identifier& tbl) override;

  brig::boost::box get_mbr(const table_def& tbl, const std::string& col) override;
  std::shared_ptr<rowset> select(const table_def& tbl) override;

  table_def fit_to_create(const table_def&) override  { throw std::runtime_error("OSM error"); }
  void create(const table_def&) override  { throw std::runtime_error("OSM error"); }
  void drop(const table_def&) override  { throw std::runtime_error("OSM error"); }
  
  pyramid_def fit_to_reg(const pyramid_def&) override  { throw std::runtime_error("OSM error"); }
  void reg(const pyramid_def&) override  { throw std::runtime_error("OSM error"); }
  void unreg(const pyramid_def&) override  { throw std::runtime_error("OSM error"); }

  std::shared_ptr<inserter> get_inserter(const table_def&) override  { throw std::runtime_error("OSM error"); }
}; // connection

inline void connection::parse_table(const std::string& tbl, size_t& lr, int& z)
{
  for (lr = 0; lr < m_lrs.size(); ++lr)
  {
    z = m_lrs[lr]->table_to_zoom(tbl);
    if (z >= 0) return;
  }
  throw std::runtime_error("OSM error");
}

inline connection::connection()
{
  using namespace std;
  using namespace detail;
  m_lrs.push_back(make_shared<layer_aerial>());
  m_lrs.push_back(make_shared<layer_cycle>());
  m_lrs.push_back(make_shared<layer_osm>());
  m_lrs.push_back(make_shared<layer_mapquest>());
}

inline std::vector<identifier> connection::get_tables()
{
  using namespace detail;
  std::vector<identifier> res;
  for (size_t lr(0); lr < m_lrs.size(); ++lr)
    for (int z(m_lrs[lr]->max_zoom()); z >= 0; --z)
    {
      identifier id;
      id.name = m_lrs[lr]->zoom_to_table(z);
      res.push_back(id);
    }
  return res;
}

inline std::vector<identifier> connection::get_geometry_layers()
{
  using namespace detail;
  std::vector<identifier> res;
  for (size_t lr(0); lr < m_lrs.size(); ++lr)
    for (int z(m_lrs[lr]->max_zoom()); z >= 0; --z)
    {
      identifier id;
      id.name = m_lrs[lr]->zoom_to_table(z);
      id.qualifier = layer::column_geometry();
      res.push_back(id);
    }
  return res;
}

inline std::vector<pyramid_def> connection::get_raster_layers()
{
  using namespace detail;
  std::vector<pyramid_def> res;
  for (size_t lr(0); lr < m_lrs.size(); ++lr)
  {
    pyramid_def pyramid;
    pyramid.id.name = m_lrs[lr]->zoom_to_table(m_lrs[lr]->max_zoom());
    pyramid.id.qualifier = layer::column_raster();
    for (int z(m_lrs[lr]->max_zoom()); z >= 0; --z)
    {
      auto env(tile(0, 0, z).get_mbr());
      tiling_def lvl;
      lvl.resolution_x = (env.max_corner().get<0>() - env.min_corner().get<0>()) / double(layer::Pixels);
      lvl.resolution_y = (env.max_corner().get<1>() - env.min_corner().get<1>()) / double(layer::Pixels);
      lvl.geometry.name = m_lrs[lr]->zoom_to_table(z);
      lvl.geometry.qualifier = layer::column_geometry();
      lvl.raster.name = layer::column_raster();
      lvl.raster.type = Blob;
      pyramid.levels.push_back(lvl);
    }
    res.push_back(pyramid);
  }
  return res;
}

inline table_def connection::get_table_def(const identifier& tbl)
{
  using namespace detail;

  size_t lr(0);
  int z(0);
  parse_table(tbl.name, lr, z);

  table_def res;
  res.id = tbl;

  {
    column_def col;
    col.name = layer::column_geometry();
    col.type = Geometry;
    col.epsg = 3395;
    res.columns.push_back(col);
  }

  {
    column_def col;
    col.name = layer::column_raster();
    col.type = Blob;
    res.columns.push_back(col);
  }

  index_def idx;
  idx.type = Spatial;
  idx.columns.push_back(layer::column_geometry());
  res.indexes.push_back(idx);

  return res;
}

inline brig::boost::box connection::get_mbr(const table_def& tbl, const std::string&)
{
  size_t lr(0);
  int z(0);
  parse_table(tbl.id.name, lr, z);
  return detail::tile(0, 0, 0).get_mbr();
}

inline std::shared_ptr<rowset> connection::select(const table_def& tbl)
{
  using namespace std;
  using namespace brig::boost;
  using namespace detail;

  size_t lr(0);
  int z(0);
  parse_table(tbl.id.name, lr, z);
  if (typeid(null_t) != tbl[layer::column_raster()]->query_value.type()) throw runtime_error("OSM error");

  vector<column_def> col_defs = tbl.query_columns.empty()? tbl.columns: brig::detail::get_columns(tbl.columns, tbl.query_columns);
  vector<bool> cols;
  bool lite(true);
  for (auto iter(begin(col_defs)); iter != end(col_defs); ++iter)
  {
    if (iter->name.compare(layer::column_raster()) == 0)
    {
      cols.push_back(true);
      lite = false;
    }
    else if (iter->name.compare(layer::column_geometry()) == 0)
      cols.push_back(false);
    else
      throw runtime_error("OSM error");
  }

  auto geom_col(tbl[layer::column_geometry()]);
  auto env((typeid(blob_t) == geom_col->query_value.type())? envelope(geom_from_wkb(::boost::get<blob_t>(geom_col->query_value))): tile(0, 0, 0).get_mbr());
  if (lite)
    return make_shared<rowset_lite>(cols.size(), z, env, tbl.query_rows);
  else
    return make_shared<rowset>(m_lrs[lr], cols, z, env, tbl.query_rows);
} // connection::

} } // brig::osm

#endif // BRIG_OSM_CONNECTION_HPP
