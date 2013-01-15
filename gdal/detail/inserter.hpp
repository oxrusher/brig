// Andrew Naplavkov

#ifndef BRIG_GDAL_DETAIL_INSERTER_HPP
#define BRIG_GDAL_DETAIL_INSERTER_HPP

#include <brig/detail/get_columns.hpp>
#include <brig/detail/raii.hpp>
#include <brig/gdal/detail/datasource.hpp>
#include <brig/gdal/detail/lib.hpp>
#include <brig/inserter.hpp>
#include <brig/numeric_cast.hpp>
#include <brig/table_definition.hpp>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace brig { namespace gdal { namespace detail {

class inserter : public brig::inserter {
  std::unique_ptr<datasource> m_ds;
  OGRLayerH m_lr;
  OGRFeatureDefnH m_feature_def;
  OGRSpatialReferenceH m_sr;
  std::vector<int> m_fields;
public:
  inserter(datasource* ds, const table_definition& tbl);
  void insert(std::vector<variant>& row) override;
  void flush() override;
}; // inserter

inline inserter::inserter(datasource* ds, const table_definition& tbl) : m_ds(ds)
{
  using namespace std;

  m_lr = lib::singleton().p_OGR_DS_GetLayerByName(*m_ds, tbl.id.name.c_str());
  if (!m_lr) throw runtime_error("GDAL error");
  m_feature_def = lib::singleton().p_OGR_L_GetLayerDefn(m_lr);
  if (!m_feature_def) throw runtime_error("GDAL error");
  m_sr = lib::singleton().p_OGR_L_GetSpatialRef(m_lr);

  vector<column_definition> cols = tbl.query_columns.empty()? tbl.columns: brig::detail::get_columns(tbl.columns, tbl.query_columns);
  m_fields.resize(cols.size(), -1);

  for (int i(0), count(lib::singleton().p_OGR_FD_GetFieldCount(m_feature_def)); i < count; ++i)
  {
    OGRFieldDefnH field_def(lib::singleton().p_OGR_FD_GetFieldDefn(m_feature_def, i));
    if (!field_def) throw runtime_error("GDAL error");
    auto col(find_column(begin(cols), end(cols), lib::singleton().p_OGR_Fld_GetNameRef(field_def)));
    if (!col || Geometry == col->type) continue;
    auto pos(distance(cols.data(), col));
    m_fields[pos] = i;
  }
}

inline void inserter::insert(std::vector<variant>& row)
{
  using namespace std;

  if (row.size() != m_fields.size()) throw runtime_error("GDAL error");
  auto feature(brig::detail::make_raii(lib::singleton().p_OGR_F_Create(m_feature_def), lib::singleton().p_OGR_F_Destroy));
  if (!feature) throw runtime_error("GDAL error");

  for (size_t i(0); i < m_fields.size(); ++i)
  {
    if (m_fields[i] < 0)
    {
      blob_t& wkb(::boost::get<blob_t>(row[i]));
      OGRGeometryH geom(0);
      lib::check(lib::singleton().p_OGR_G_CreateFromWkb((unsigned char*)wkb.data(), m_sr, &geom, int(wkb.size())));
      lib::singleton().p_OGR_F_SetGeometryDirectly(feature, geom);
    }
    else if (typeid(null_t) == row[i].type())
      lib::singleton().p_OGR_F_UnsetField(feature, m_fields[i]);
    else if (typeid(int16_t) == row[i].type() || typeid(int32_t) == row[i].type() || typeid(int64_t) == row[i].type())
    {
      int val(0);
      if (!numeric_cast(row[i], val)) throw runtime_error("GDAL error");
      lib::singleton().p_OGR_F_SetFieldInteger(feature, m_fields[i], val);
    }
    else if (typeid(float) == row[i].type() || typeid(double) == row[i].type())
    {
      double val(0);
      if (!numeric_cast(row[i], val)) throw runtime_error("GDAL error");
      lib::singleton().p_OGR_F_SetFieldDouble(feature, m_fields[i], val);
    }
    else if (typeid(string) == row[i].type())
    {
      string& str(::boost::get<string>(row[i]));
      lib::singleton().p_OGR_F_SetFieldString(feature, m_fields[i], str.c_str());
    }
    else if (typeid(blob_t) == row[i].type())
    {
      blob_t& blob(::boost::get<blob_t>(row[i]));
      lib::singleton().p_OGR_F_SetFieldBinary(feature, m_fields[i], int(blob.size()), (GByte*)blob.data());
    }
    else
      throw runtime_error("GDAL error");
  }

  lib::check(lib::singleton().p_OGR_L_CreateFeature(m_lr, feature));
}

inline void inserter::flush()
{
  lib::check(lib::singleton().p_OGR_L_SyncToDisk(m_lr));
  lib::check(lib::singleton().p_OGR_DS_SyncToDisk(*m_ds));
} // inserter::

} } } // brig::gdal::detail

#endif // BRIG_GDAL_DETAIL_INSERTER_HPP
