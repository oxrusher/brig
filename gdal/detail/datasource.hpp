// Andrew Naplavkov

#ifndef BRIG_GDAL_DETAIL_DATASOURCE_HPP
#define BRIG_GDAL_DETAIL_DATASOURCE_HPP

#include <algorithm>
#include <brig/boost/envelope.hpp>
#include <brig/boost/geom_from_wkb.hpp>
#include <brig/boost/geometry.hpp>
#include <brig/detail/get_columns.hpp>
#include <brig/detail/raii.hpp>
#include <brig/gdal/detail/lib.hpp>
#include <brig/proj/shared_pj.hpp>
#include <brig/rowset.hpp>
#include <brig/string_cast.hpp>
#include <brig/table_def.hpp>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <vector>

namespace brig { namespace gdal { namespace detail {

class datasource : public brig::rowset {
  OGRDataSourceH m_ds;
  OGRLayerH m_lr;
  std::vector<int> m_cols;
  int m_rows;

public:
  datasource(const std::string& ds, bool writable);
  datasource(const std::string& drv, const std::string& ds);
  ~datasource() override  { lib::singleton().p_OGR_DS_Destroy(m_ds); }

  operator OGRDataSourceH() const  { return m_ds; }

  std::vector<identifier> get_tables();
  std::vector<identifier> get_geometry_layers();
  table_def get_table_def(const identifier& tbl);

  brig::boost::box get_mbr(const table_def& tbl, const std::string& col);
  void select(const table_def& tbl);

  std::vector<std::string> columns() override;
  bool fetch(std::vector<variant>& row) override;

  void create(const table_def& tbl);
  void drop(const table_def& tbl);
}; // datasource

inline datasource::datasource(const std::string& ds, bool writable) : m_ds(0), m_lr(0), m_rows(-1)
{
  if (lib::singleton().empty()) throw std::runtime_error("GDAL error");
  m_ds = lib::singleton().p_OGROpen(ds.c_str(), writable, 0);
  if (!m_ds) throw std::runtime_error("GDAL error");
}

inline datasource::datasource(const std::string& drv_, const std::string& ds_) : m_ds(0), m_lr(0), m_rows(-1)
{
  if (lib::singleton().empty()) throw std::runtime_error("GDAL error");
  OGRSFDriverH drv(lib::singleton().p_OGRGetDriverByName(drv_.c_str()));
  if (!drv) throw std::runtime_error("GDAL error");
  m_ds = lib::singleton().p_OGR_Dr_CreateDataSource(drv, ds_.c_str(), 0);
  if (!m_ds) throw std::runtime_error("GDAL error");
}

inline std::vector<identifier> datasource::get_tables()
{
  using namespace std;
  vector<identifier> res;
  for (int i(0), count(lib::singleton().p_OGR_DS_GetLayerCount(m_ds)); i < count; ++i)
  {
    OGRLayerH lr(lib::singleton().p_OGR_DS_GetLayer(m_ds, i));
    if (!lr) throw runtime_error("GDAL error");
    identifier id;
    id.name = lib::singleton().p_OGR_L_GetName(lr);
    res.push_back(id);
  }
  return res;
}

inline std::vector<identifier> datasource::get_geometry_layers()
{
  using namespace std;
  vector<identifier> res;
  for (int i(0), count(lib::singleton().p_OGR_DS_GetLayerCount(m_ds)); i < count; ++i)
  {
    OGRLayerH lr(lib::singleton().p_OGR_DS_GetLayer(m_ds, i));
    if (!lr) throw runtime_error("GDAL error");
    identifier id;
    id.name = lib::singleton().p_OGR_L_GetName(lr);
    id.qualifier = lib::ogr_geom_wkb();
    res.push_back(id);
  }
  return res;
}

inline table_def datasource::get_table_def(const identifier& tbl)
{
  using namespace std;

  OGRLayerH lr(lib::singleton().p_OGR_DS_GetLayerByName(m_ds, tbl.name.c_str()));
  if (!lr) throw runtime_error("GDAL error");

  table_def res;
  res.id.name = tbl.name;

  column_def col;
  col.name = lib::ogr_geom_wkb();
  col.type = Geometry;
  OGRSpatialReferenceH sr(lib::singleton().p_OGR_L_GetSpatialRef(lr));
  if (sr)
  {
    lib::singleton().p_OSRAutoIdentifyEPSG(sr);
    const char* name(lib::singleton().p_OSRGetAuthorityName(sr, 0));
    const char* code(lib::singleton().p_OSRGetAuthorityCode(sr, 0));
    if (name && code && string(name).compare("EPSG") == 0)
      col.epsg = atoi(code);
    else
    {
      char* proj(0);
      if (OGRERR_NONE == lib::singleton().p_OSRExportToProj4(sr, &proj) && proj)
      {
        col.proj = proj;
        lib::singleton().p_OGRFree(proj);
      }
    }
  }
  res.columns.push_back(col);

  OGRFeatureDefnH feature_def(lib::singleton().p_OGR_L_GetLayerDefn(lr));
  if (!feature_def) throw runtime_error("GDAL error");
  for (int i(0), count(lib::singleton().p_OGR_FD_GetFieldCount(feature_def)); i < count; ++i)
  {
    OGRFieldDefnH field_def(lib::singleton().p_OGR_FD_GetFieldDefn(feature_def, i));
    if (!field_def) throw runtime_error("GDAL error");
    col.name = lib::singleton().p_OGR_Fld_GetNameRef(field_def);
    if (col.name.empty() || col.name.compare(lib::ogr_geom_wkb()) == 0) throw runtime_error("GDAL name error");
    switch (lib::singleton().p_OGR_Fld_GetType(field_def))
    {
    default: col.type = VoidColumn; break;
    case OFTInteger: col.type = Integer; break;
    case OFTReal: col.type = Double; break;
    case OFTDate:
    case OFTTime:
    case OFTDateTime:
    case OFTString: col.type = String; break;
    case OFTBinary: col.type = Blob; break;
    }
    res.columns.push_back(col);
  }
  return res;
}

inline brig::boost::box datasource::get_mbr(const table_def& tbl, const std::string&)
{
  using namespace std;
  using namespace brig::boost;

  OGRLayerH lr(lib::singleton().p_OGR_DS_GetLayerByName(m_ds, tbl.id.name.c_str()));
  if (!lr) throw runtime_error("GDAL error");

  OGREnvelope env;
  lib::check(lib::singleton().p_OGR_L_GetExtent(lr, &env, 1));
  return box(point(env.MinX, env.MinY), point(env.MaxX, env.MaxY));
}

inline void datasource::select(const table_def& tbl)
{
  using namespace std;
  using namespace brig::boost;

  m_lr = 0;
  m_cols.clear();
  m_rows = -1;

  OGRLayerH lr(lib::singleton().p_OGR_DS_GetLayerByName(m_ds, tbl.id.name.c_str()));
  if (!lr) throw runtime_error("GDAL error");
  OGRFeatureDefnH feature_def(lib::singleton().p_OGR_L_GetLayerDefn(lr));
  if (!feature_def) throw runtime_error("GDAL error");
  vector<column_def> cols = tbl.query_columns.empty()? tbl.columns: brig::detail::get_columns(tbl.columns, tbl.query_columns);
  for (auto col(begin(cols)); col != end(cols); ++col)
  {
    if (Geometry == col->type)
      m_cols.push_back(-1);
    else
    {
      m_cols.push_back(lib::singleton().p_OGR_FD_GetFieldIndex(feature_def, col->name.c_str()));
      if (m_cols.back() < 0) throw runtime_error("GDAL error");
    }
  }
  m_rows = tbl.query_rows;

  string attribute_filter;
  for (auto col(begin(tbl.columns)); col != end(tbl.columns); ++col)
  {
    if (Geometry == col->type)
    {
      if (typeid(null_t) == col->query_value.type())
        lib::singleton().p_OGR_L_SetSpatialFilter(lr, 0);
      else
      {
        const box env(envelope(geom_from_wkb(::boost::get<blob_t>(col->query_value))));
        const double xmin(env.min_corner().get<0>()), ymin(env.min_corner().get<1>()), xmax(env.max_corner().get<0>()), ymax(env.max_corner().get<1>());
        lib::singleton().p_OGR_L_SetSpatialFilterRect(lr, xmin, ymin, xmax, ymax);
      }
    }
    else if (typeid(null_t) != col->query_value.type())
    {
      if (!attribute_filter.empty()) attribute_filter += " AND ";
      attribute_filter += col->name + " = " + string_cast<char>(col->query_value);
    }
  }
  lib::check(lib::singleton().p_OGR_L_SetAttributeFilter(lr, attribute_filter.empty()? 0: attribute_filter.c_str()));

  lib::singleton().p_OGR_L_ResetReading(lr);

  swap(lr, m_lr);
}

inline std::vector<std::string> datasource::columns()
{
  using namespace std;

  if (!m_lr) return vector<string>();
  vector<string> cols;
  OGRFeatureDefnH feature_def(lib::singleton().p_OGR_L_GetLayerDefn(m_lr));
  if (!feature_def) throw runtime_error("GDAL error");
  for (size_t i(0); i < m_cols.size(); ++i)
  {
    string col;
    if (m_cols[i] < 0)
      col = lib::ogr_geom_wkb();
    else
    {
      OGRFieldDefnH field_def(lib::singleton().p_OGR_FD_GetFieldDefn(feature_def, m_cols[i]));
      if (!field_def) throw runtime_error("GDAL error");
      col = lib::singleton().p_OGR_Fld_GetNameRef(field_def);
    }
    if (col.empty()) throw runtime_error("GDAL error");
    cols.push_back(col);
  }
  return cols;
}

inline bool datasource::fetch(std::vector<variant>& row)
{
  using namespace std;

  if (!m_lr || m_rows == 0) return false;
  if (m_rows > 0) --m_rows;
  auto feature(brig::detail::make_raii(lib::singleton().p_OGR_L_GetNextFeature(m_lr), lib::singleton().p_OGR_F_Destroy));
  if (!feature) return false;

  row.resize(m_cols.size());
  for (size_t i(0); i < m_cols.size(); ++i)
  {
    if (m_cols[i] < 0)
    {
      OGRGeometryH geom(lib::singleton().p_OGR_F_GetGeometryRef(feature));
      int size(lib::singleton().p_OGR_G_WkbSize(geom));
      if (size > 0)
      {
        row[i] = blob_t();
        blob_t& blob = ::boost::get<blob_t>(row[i]);
        blob.resize(size_t(size));
        lib::singleton().p_OGR_G_ExportToWkb(geom, wkbXDR, (unsigned char*)blob.data());
      }
      else
        row[i] = null_t();
    }
    else
    {
      OGRFieldDefnH field_def(lib::singleton().p_OGR_F_GetFieldDefnRef(feature, m_cols[i]));
      if (!field_def) throw runtime_error("GDAL error");
      switch (lib::singleton().p_OGR_Fld_GetType(field_def))
      {
      default: throw runtime_error("GDAL error");
      case OFTInteger: row[i] = lib::singleton().p_OGR_F_GetFieldAsInteger(feature, m_cols[i]); break;
      case OFTReal: row[i] = lib::singleton().p_OGR_F_GetFieldAsDouble(feature, m_cols[i]); break;
      case OFTDate:
      case OFTTime:
      case OFTDateTime:
      case OFTString: row[i] = string(lib::singleton().p_OGR_F_GetFieldAsString(feature, m_cols[i])); break;
      case OFTBinary:
        {
        int size(0);
        GByte* bytes(lib::singleton().p_OGR_F_GetFieldAsBinary(feature, m_cols[i], &size));
        if (size > 0)
        {
          row[i] = blob_t();
          blob_t& blob = ::boost::get<blob_t>(row[i]);
          blob.resize(size_t(size));
          memcpy((GByte*)blob.data(), bytes, size_t(size));
        }
        else
          row[i] = null_t();
        break;
        }
      }
    }
  }
  return true;
}

inline void datasource::create(const table_def& tbl)
{
  using namespace std;

  auto geom_col(find_if(begin(tbl.columns), end(tbl.columns), [](const column_def& col){ return Geometry == col.type; }));
  if (geom_col == end(tbl.columns)) throw runtime_error("GDAL error");
  auto srs(brig::detail::make_raii
    ( lib::singleton().p_OSRNewSpatialReference("GEOGCS[\"WGS 84\", DATUM[\"WGS_1984\", SPHEROID[\"WGS 84\",6378137,298.257223563]], PRIMEM[\"Greenwich\",0], UNIT[\"degree\",0.01745329251994328]]")
    , lib::singleton().p_OSRDestroySpatialReference)
    );
  if (!srs) throw runtime_error("GDAL error");

  string proj;
  if (geom_col->epsg > 0)
  {
    brig::proj::shared_pj pj(geom_col->epsg);
    proj = pj.get_def();
  }
  else if (!geom_col->proj.empty())
    proj = geom_col->proj;
  lib::check(lib::singleton().p_OSRImportFromProj4(srs, proj.c_str()));

  OGRLayerH lr(lib::singleton().p_OGR_DS_CreateLayer(m_ds, tbl.id.name.c_str(), srs, wkbUnknown, 0));
  if (!lr) throw runtime_error("GDAL error");

  for (auto col(begin(tbl.columns)); col != end(tbl.columns); ++col)
  {
    if (Geometry == col->type) continue;
    OGRFieldType type(OFTString);
    switch (col->type)
    {
    case VoidColumn:
    case Geometry: throw runtime_error("GDAL error");
    case Blob: type = OFTBinary; break;
    case Double: type = OFTReal; break;
    case Integer: type = OFTInteger; break;
    case String: type = OFTString; break;
    };
    auto fld(brig::detail::make_raii(lib::singleton().p_OGR_Fld_Create(col->name.c_str(), type), lib::singleton().p_OGR_Fld_Destroy));
    lib::check(lib::singleton().p_OGR_L_CreateField(lr, fld, true));
  }
  lib::check(lib::singleton().p_OGR_L_SyncToDisk(lr));
  lib::check(lib::singleton().p_OGR_DS_SyncToDisk(m_ds));
}

inline void datasource::drop(const table_def& tbl)
{
  for (int i(0), count(lib::singleton().p_OGR_DS_GetLayerCount(m_ds)); i < count; ++i)
  {
    OGRLayerH lr(lib::singleton().p_OGR_DS_GetLayer(m_ds, i));
    if (!lr) throw std::runtime_error("GDAL error");
    if (tbl.id.name.compare(lib::singleton().p_OGR_L_GetName(lr)) == 0)
    {
      lib::check(lib::singleton().p_OGR_DS_DeleteLayer(m_ds, i));
      lib::check(lib::singleton().p_OGR_DS_SyncToDisk(m_ds));
      return;
    }
  }
  throw std::runtime_error("GDAL error");
} // datasource::

} } } // brig::gdal::detail

#endif // BRIG_GDAL_DETAIL_DATASOURCE_HPP
