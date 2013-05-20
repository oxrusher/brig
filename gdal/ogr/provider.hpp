// Andrew Naplavkov

#ifndef BRIG_GDAL_OGR_PROVIDER_HPP
#define BRIG_GDAL_OGR_PROVIDER_HPP

#include <algorithm>
#include <brig/boost/geometry.hpp>
#include <brig/gdal/detail/lib.hpp>
#include <brig/gdal/ogr/detail/datasource_allocator.hpp>
#include <brig/gdal/ogr/detail/inserter.hpp>
#include <brig/gdal/ogr/detail/rowset.hpp>
#include <brig/global.hpp>
#include <brig/proj/shared_pj.hpp>
#include <brig/provider.hpp>
#include <cstdlib>
#include <memory>
#include <iterator>
#include <stdexcept>
#include <string>

namespace brig { namespace gdal { namespace ogr {

class provider : public brig::provider {
  detail::datasource_allocator m_allocator;
  std::string m_fitted_id;

public:
  provider(const std::string& ds, const std::string& drv = std::string(), const std::string& fitted_id = std::string()) : m_allocator(ds, drv), m_fitted_id(fitted_id)  {}

  std::vector<identifier> get_tables() override;
  std::vector<identifier> get_geometry_layers() override;
  std::vector<pyramid_def> get_raster_layers() override  { return std::vector<pyramid_def>(); }
  table_def get_table_def(const identifier& tbl) override;
  boost::box get_extent(const table_def& tbl) override;
  std::shared_ptr<rowset> select(const table_def& tbl) override;

  bool is_readonly() override;
  table_def fit_to_create(const table_def& tbl) override;
  void create(const table_def& tbl) override;
  void drop(const table_def& tbl) override;
  pyramid_def fit_to_reg(const pyramid_def&) override  { throw std::runtime_error("OGR error"); }
  void reg(const pyramid_def&) override  { throw std::runtime_error("OGR error"); }
  void unreg(const pyramid_def&) override  { throw std::runtime_error("OGR error"); }
  std::shared_ptr<inserter> get_inserter(const table_def& tbl) override;
}; // provider

inline std::vector<identifier> provider::get_tables()
{
  using namespace std;
  using namespace gdal::detail;

  detail::datasource ds(m_allocator.allocate(false));
  vector<identifier> res;
  for (int i(0), count(lib::singleton().p_OGR_DS_GetLayerCount(ds)); i < count; ++i)
  {
    OGRLayerH lr(lib::singleton().p_OGR_DS_GetLayer(ds, i));
    if (!lr) throw runtime_error("OGR error");
    identifier id;
    id.name = lib::singleton().p_OGR_L_GetName(lr);
    res.push_back(id);
  }
  return res;
}

inline std::vector<identifier> provider::get_geometry_layers()
{
  using namespace std;
  using namespace gdal::detail;

  detail::datasource ds(m_allocator.allocate(false));
  vector<identifier> res;
  for (int i(0), count(lib::singleton().p_OGR_DS_GetLayerCount(ds)); i < count; ++i)
  {
    OGRLayerH lr(lib::singleton().p_OGR_DS_GetLayer(ds, i));
    if (!lr) throw runtime_error("OGR error");
    identifier id;
    id.name = lib::singleton().p_OGR_L_GetName(lr);
    id.qualifier = ColumnNameWkb;
    res.push_back(id);
  }
  return res;
}

inline table_def provider::get_table_def(const identifier& tbl)
{
  using namespace std;
  using namespace gdal::detail;

  detail::datasource ds(m_allocator.allocate(false));
  OGRLayerH lr(lib::singleton().p_OGR_DS_GetLayerByName(ds, tbl.name.c_str()));
  if (!lr) throw runtime_error("OGR error");

  table_def res;
  res.id.name = tbl.name;

  column_def col;
  col.name = ColumnNameWkb;
  col.type = column_type::Geometry;
  OGRSpatialReferenceH srs(lib::singleton().p_OGR_L_GetSpatialRef(lr));
  if (srs)
  {
    lib::singleton().p_OSRAutoIdentifyEPSG(srs);
    const char* name(lib::singleton().p_OSRGetAuthorityName(srs, 0));
    const char* code(lib::singleton().p_OSRGetAuthorityCode(srs, 0));
    if (name && code && string(name).compare("EPSG") == 0)
      col.epsg = atoi(code);
    else
    {
      char* proj(0);
      if (OGRERR_NONE == lib::singleton().p_OSRExportToProj4(srs, &proj) && proj)
      {
        col.proj = proj;
        lib::singleton().p_OGRFree(proj);
      }
    }
  }
  res.columns.push_back(col);

  OGRFeatureDefnH feature_def(lib::singleton().p_OGR_L_GetLayerDefn(lr));
  if (!feature_def) throw runtime_error("OGR error");
  for (int i(0), count(lib::singleton().p_OGR_FD_GetFieldCount(feature_def)); i < count; ++i)
  {
    OGRFieldDefnH field_def(lib::singleton().p_OGR_FD_GetFieldDefn(feature_def, i));
    if (!field_def) throw runtime_error("OGR error");
    col.name = lib::singleton().p_OGR_Fld_GetNameRef(field_def);
    if (col.name.empty() || col.name.compare(ColumnNameWkb) == 0) throw runtime_error("OGR name error");
    switch (lib::singleton().p_OGR_Fld_GetType(field_def))
    {
    default: col.type = column_type::Void; break;
    case OFTInteger: col.type = column_type::Integer; break;
    case OFTReal: col.type = column_type::Double; break;
    case OFTDate:
    case OFTTime:
    case OFTDateTime:
    case OFTString: col.type = column_type::String; break;
    case OFTBinary: col.type = column_type::Blob; break;
    }
    res.columns.push_back(col);
  }
  return res;
}

inline boost::box provider::get_extent(const table_def& tbl)
{
  using namespace std;
  using namespace brig::boost;
  using namespace gdal::detail;

  detail::datasource ds(m_allocator.allocate(false));
  OGRLayerH lr(lib::singleton().p_OGR_DS_GetLayerByName(ds, tbl.id.name.c_str()));
  if (!lr) throw runtime_error("OGR error");

  OGREnvelope env;
  lib::check(lib::singleton().p_OGR_L_GetExtent(lr, &env, 1));
  return box(point(env.MinX, env.MinY), point(env.MaxX, env.MaxY));
}

inline std::shared_ptr<rowset> provider::select(const table_def& tbl)
{
  return std::make_shared<detail::rowset>(m_allocator, tbl);
}

inline bool provider::is_readonly()
{
  try
  {
    m_allocator.allocate(true); // OGR_Dr_CreateDataSource() or OGROpen(writable)
    m_allocator.allocate(true); // OGROpen(writable)
    return false;
  }
  catch (const std::exception&)
  {
    return true;
  }
}

inline table_def provider::fit_to_create(const table_def& tbl)
{
  using namespace gdal::detail;

  table_def res;
  res.id.name = m_fitted_id.empty()? tbl.id.name: m_fitted_id;
  for (const auto& col: tbl.columns)
  {
    column_def fitted_col;
    fitted_col.type = col.type;
    switch (fitted_col.type)
    {
    case column_type::Geometry:
      fitted_col.name = ColumnNameWkb;
      fitted_col.epsg = col.epsg;
      fitted_col.proj = col.proj;
      break;
    default:
      fitted_col.name = col.name;
      break;
    }
    res.columns.push_back(fitted_col);
  }
  return res;
}

inline void provider::create(const table_def& tbl)
{
  using namespace std;
  using namespace gdal::detail;

  detail::datasource ds(m_allocator.allocate(true));
  if (lib::singleton().p_OGR_DS_GetLayerByName(ds, tbl.id.name.c_str())) throw runtime_error("OGR error");

  auto geom_col(find_if(begin(tbl.columns), end(tbl.columns), [](const column_def& col){ return column_type::Geometry == col.type; }));
  if (geom_col == end(tbl.columns)) throw runtime_error("OGR error");
  auto del = [](void* ptr) { lib::singleton().p_OSRDestroySpatialReference(OGRSpatialReferenceH(ptr)); };
  unique_ptr<void, decltype(del)> srs(lib::singleton().p_OSRNewSpatialReference("GEOGCS[\"WGS 84\", DATUM[\"WGS_1984\", SPHEROID[\"WGS 84\",6378137,298.257223563]], PRIMEM[\"Greenwich\",0], UNIT[\"degree\",0.01745329251994328]]"), del);
  if (!srs.get()) throw runtime_error("OGR error");

  string proj;
  if (geom_col->epsg > 0)
  {
    brig::proj::shared_pj pj(geom_col->epsg);
    proj = pj.get_def();
  }
  else if (!geom_col->proj.empty())
    proj = geom_col->proj;
  lib::check(lib::singleton().p_OSRImportFromProj4(srs.get(), proj.c_str()));

  OGRLayerH lr(lib::singleton().p_OGR_DS_CreateLayer(ds, tbl.id.name.c_str(), srs.get(), wkbUnknown, 0));
  if (!lr) throw runtime_error("OGR error");

  for (const auto& col: tbl.columns)
  {
    if (column_type::Geometry == col.type) continue;
    OGRFieldType type(OFTString);
    switch (col.type)
    {
    case column_type::Void:
    case column_type::Geometry: throw runtime_error("OGR error");
    case column_type::Blob: type = OFTBinary; break;
    case column_type::Double: type = OFTReal; break;
    case column_type::Integer: type = OFTInteger; break;
    case column_type::String: type = OFTString; break;
    };
    auto del = [](void* ptr) { lib::singleton().p_OGR_Fld_Destroy(OGRFieldDefnH(ptr)); };
    unique_ptr<void, decltype(del)> fld(lib::singleton().p_OGR_Fld_Create(col.name.c_str(), type), del);
    lib::check(lib::singleton().p_OGR_L_CreateField(lr, fld.get(), true));
  }
  lib::check(lib::singleton().p_OGR_L_SyncToDisk(lr));
  lib::check(lib::singleton().p_OGR_DS_SyncToDisk(ds));
}

inline void provider::drop(const table_def& tbl)
{
  using namespace std;
  using namespace gdal::detail;

  detail::datasource ds(m_allocator.allocate(true));
  for (int i(0), count(lib::singleton().p_OGR_DS_GetLayerCount(ds)); i < count; ++i)
  {
    OGRLayerH lr(lib::singleton().p_OGR_DS_GetLayer(ds, i));
    if (!lr) throw runtime_error("OGR error");
    if (tbl.id.name.compare(lib::singleton().p_OGR_L_GetName(lr)) == 0)
    {
      lib::check(lib::singleton().p_OGR_DS_DeleteLayer(ds, i));
      lib::check(lib::singleton().p_OGR_DS_SyncToDisk(ds));
      return;
    }
  }
  throw runtime_error("OGR error");
}

inline std::shared_ptr<inserter> provider::get_inserter(const table_def& tbl)
{
  return std::make_shared<detail::inserter>(m_allocator, tbl);
} // provider::

} } } // brig::gdal::ogr

#endif // BRIG_GDAL_OGR_PROVIDER_HPP
