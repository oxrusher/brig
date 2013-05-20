// Andrew Naplavkov

#ifndef BRIG_GDAL_OGR_DETAIL_ROWSET_HPP
#define BRIG_GDAL_OGR_DETAIL_ROWSET_HPP

#include <brig/boost/envelope.hpp>
#include <brig/boost/geom_from_wkb.hpp>
#include <brig/boost/geometry.hpp>
#include <brig/detail/get_columns.hpp>
#include <brig/gdal/detail/lib.hpp>
#include <brig/gdal/ogr/detail/datasource_allocator.hpp>
#include <brig/global.hpp>
#include <brig/rowset.hpp>
#include <brig/string_cast.hpp>
#include <brig/table_def.hpp>
#include <cstring>
#include <stdexcept>
#include <vector>

namespace brig { namespace gdal { namespace ogr { namespace detail {

class rowset : public brig::rowset {
  datasource m_ds;
  OGRLayerH m_lr;
  std::vector<int> m_cols;
  int m_rows;

  int m_interleaved_reading_lr;
  bool m_interleaved_reading_non_empty;
  std::shared_ptr<void> interleaved_reading_next();

public:
  rowset(datasource_allocator allocator, const table_def& tbl);
  std::vector<std::string> columns() override;
  bool fetch(std::vector<variant>& row) override;
}; // rowset

inline rowset::rowset(datasource_allocator allocator, const table_def& tbl)
  : m_ds(allocator.allocate(false)), m_lr(0)
  , m_interleaved_reading_lr(0), m_interleaved_reading_non_empty(false)
{
  using namespace std;
  using namespace brig::boost;
  using namespace gdal::detail;

  OGRLayerH lr(lib::singleton().p_OGR_DS_GetLayerByName(m_ds, tbl.id.name.c_str()));
  if (!lr) throw runtime_error("OGR error");
  OGRFeatureDefnH feature_def(lib::singleton().p_OGR_L_GetLayerDefn(lr));
  if (!feature_def) throw runtime_error("OGR error");
  vector<column_def> cols = tbl.query_columns.empty()? tbl.columns: brig::detail::get_columns(tbl.columns, tbl.query_columns);
  for (const auto& col: cols)
  {
    if (column_type::Geometry == col.type)
      m_cols.push_back(-1);
    else
    {
      m_cols.push_back(lib::singleton().p_OGR_FD_GetFieldIndex(feature_def, col.name.c_str()));
      if (m_cols.back() < 0) throw runtime_error("OGR error");
    }
  }
  m_rows = tbl.query_rows;

  string attribute_filter;
  for (const auto& col: tbl.columns)
  {
    if (column_type::Geometry == col.type)
    {
      if (typeid(null_t) == col.query_value.type())
        lib::singleton().p_OGR_L_SetSpatialFilter(lr, 0);
      else
      {
        const box env(envelope(geom_from_wkb(::boost::get<blob_t>(col.query_value))));
        const double xmin(env.min_corner().get<0>()), ymin(env.min_corner().get<1>()), xmax(env.max_corner().get<0>()), ymax(env.max_corner().get<1>());
        lib::singleton().p_OGR_L_SetSpatialFilterRect(lr, xmin, ymin, xmax, ymax);
      }
    }
    else if (typeid(null_t) != col.query_value.type())
    {
      if (!attribute_filter.empty()) attribute_filter += " AND ";
      attribute_filter += col.name + " = " + string_cast<char>(col.query_value);
    }
  }
  lib::check(lib::singleton().p_OGR_L_SetAttributeFilter(lr, attribute_filter.empty()? 0: attribute_filter.c_str()));

  swap(lr, m_lr);
}

inline std::vector<std::string> rowset::columns()
{
  using namespace std;
  using namespace gdal::detail;

  if (!m_lr) return vector<string>();
  vector<string> cols;
  OGRFeatureDefnH feature_def(lib::singleton().p_OGR_L_GetLayerDefn(m_lr));
  if (!feature_def) throw runtime_error("OGR error");
  for (size_t i(0); i < m_cols.size(); ++i)
  {
    string col;
    if (m_cols[i] < 0)
      col = ColumnNameWkb;
    else
    {
      OGRFieldDefnH field_def(lib::singleton().p_OGR_FD_GetFieldDefn(feature_def, m_cols[i]));
      if (!field_def) throw runtime_error("OGR error");
      col = lib::singleton().p_OGR_Fld_GetNameRef(field_def);
    }
    if (col.empty()) throw runtime_error("OGR error");
    cols.push_back(col);
  }
  return cols;
}

std::shared_ptr<void> rowset::interleaved_reading_next()
{
  using namespace std;
  using namespace gdal::detail;

  while (true)
  {
    if (m_interleaved_reading_lr >= lib::singleton().p_OGR_DS_GetLayerCount(m_ds)) return std::shared_ptr<void>();

    OGRLayerH lr(lib::singleton().p_OGR_DS_GetLayer(m_ds, m_interleaved_reading_lr));
    if (!lr) throw runtime_error("OGR error");

    std::shared_ptr<void> feature;
    while( bool(feature = shared_ptr<void>(lib::singleton().p_OGR_L_GetNextFeature(lr), [](void* ptr) { lib::singleton().p_OGR_F_Destroy(OGRFeatureH(ptr)); })) )
    {
      m_interleaved_reading_non_empty = true;
      if (lr == m_lr) return feature;
    }

    ++m_interleaved_reading_lr;
    if (m_interleaved_reading_lr >= lib::singleton().p_OGR_DS_GetLayerCount(m_ds) && m_interleaved_reading_non_empty)
    {
      m_interleaved_reading_lr = 0;
      m_interleaved_reading_non_empty = false;
    }
  }
}

inline bool rowset::fetch(std::vector<variant>& row)
{
  using namespace std;
  using namespace gdal::detail;

  if (!m_lr || m_rows == 0) return false;
  if (m_rows > 0) --m_rows;
  auto feature(interleaved_reading_next());
  if (!bool(feature)) return false;
  row.resize(m_cols.size());
  for (size_t i(0); i < m_cols.size(); ++i)
  {
    if (m_cols[i] < 0)
    {
      OGRGeometryH geom(lib::singleton().p_OGR_F_GetGeometryRef(feature.get()));
      int size(geom? lib::singleton().p_OGR_G_WkbSize(geom): 0);
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
      OGRFieldDefnH field_def(lib::singleton().p_OGR_F_GetFieldDefnRef(feature.get(), m_cols[i]));
      if (!field_def) throw runtime_error("OGR error");
      switch (lib::singleton().p_OGR_Fld_GetType(field_def))
      {
      default: throw runtime_error("OGR error");
      case OFTInteger: row[i] = lib::singleton().p_OGR_F_GetFieldAsInteger(feature.get(), m_cols[i]); break;
      case OFTReal: row[i] = lib::singleton().p_OGR_F_GetFieldAsDouble(feature.get(), m_cols[i]); break;
      case OFTDate:
      case OFTTime:
      case OFTDateTime:
      case OFTString: row[i] = string(lib::singleton().p_OGR_F_GetFieldAsString(feature.get(), m_cols[i])); break;
      case OFTBinary:
        {
        int size(0);
        GByte* bytes(lib::singleton().p_OGR_F_GetFieldAsBinary(feature.get(), m_cols[i], &size));
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
} // rowset::

} } } } // brig::gdal::ogr::detail

#endif // BRIG_GDAL_OGR_DETAIL_ROWSET_HPP
