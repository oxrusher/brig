// Andrew Naplavkov

#ifndef BRIG_GDAL_DETAIL_ROWSET_HPP
#define BRIG_GDAL_DETAIL_ROWSET_HPP

#include <algorithm>
#include <boost/geometry/geometry.hpp>
#include <brig/boost/as_binary.hpp>
#include <brig/boost/envelope.hpp>
#include <brig/boost/geom_from_wkb.hpp>
#include <brig/boost/geometry.hpp>
#include <brig/detail/get_columns.hpp>
#include <brig/gdal/detail/dataset_allocator.hpp>
#include <brig/gdal/detail/lib.hpp>
#include <brig/gdal/detail/transform.hpp>
#include <brig/global.hpp>
#include <brig/rowset.hpp>
#include <brig/string_cast.hpp>
#include <brig/table_def.hpp>
#include <cstdint>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <vector>

namespace brig { namespace gdal { namespace detail {

class rowset : public brig::rowset {
  std::unique_ptr<dataset> m_ds;
  bool m_done;
  std::vector<bool> m_cols;
  transform m_tr;
  int m_width, m_height;
public:
  rowset(dataset_allocator* allocator, const table_def& tbl);
  std::vector<std::string> columns() override;
  bool fetch(std::vector<variant>& row) override;
}; // rowset

inline rowset::rowset(dataset_allocator* allocator, const table_def& tbl) : m_ds(allocator->allocate()), m_done(false)
{
  using namespace std;
  using namespace brig::boost;

  vector<column_def> col_defs = tbl.query_columns.empty()? tbl.columns: brig::detail::get_columns(tbl.columns, tbl.query_columns);
  for (auto iter(begin(col_defs)); iter != end(col_defs); ++iter)
  {
    if (iter->name.compare(PNG) == 0)
      m_cols.push_back(true);
    else if (iter->name.compare(WKB) == 0)
      m_cols.push_back(false);
    else
      throw runtime_error("GDAL error");
  }
  if (tbl.query_rows == 0)
    m_done = true;

  lib::check(lib::singleton().p_GDALGetGeoTransform(*m_ds, m_tr.coef));
  m_width = lib::singleton().p_GDALGetRasterXSize(*m_ds);
  m_height = lib::singleton().p_GDALGetRasterYSize(*m_ds);

  auto geom_col(tbl[WKB]);
  if ( typeid(blob_t) == geom_col->query_value.type()
    && !::boost::geometry::intersects
      ( box(point(0, 0), point(m_width, m_height))
      , envelope(m_tr.proj_to_pixel(envelope(geom_from_wkb(::boost::get<blob_t>(geom_col->query_value)))))
      )
    )
    m_done = true;
}

inline std::vector<std::string> rowset::columns()
{
  std::vector<std::string> cols;
  for (size_t i(0); i < m_cols.size(); ++i)
  {
    if (m_cols[i]) cols.push_back(PNG);
    else cols.push_back(WKB);
  }
  return cols;
}

inline bool rowset::fetch(std::vector<variant>& row)
{
  using namespace std;
  using namespace brig::boost;
  using namespace brig::detail;

  if (m_done) return false;
  m_done = true;

  const size_t count(m_cols.size());
  row.resize(count);
  for (size_t i(0); i < count; ++i)
  {
    if (m_cols[i])
    {
      auto drv(lib::singleton().p_GDALGetDriverByName("PNG"));
      if (!drv) throw runtime_error("GDAL error");
      auto file("/vsimem/" + string_cast<char>(size_t(this)) + ".png");
      {
        auto del = [](void* ptr) { lib::singleton().p_GDALClose(GDALDatasetH(ptr)); };
        unique_ptr<void, decltype(del)> ds(lib::singleton().p_GDALCreateCopy(drv, file.c_str(), *m_ds, false, 0, 0, 0), del);
        if (!ds.get()) throw runtime_error("GDAL error");
      }
      {
        vsi_l_offset len(0);
        auto del = [](void* ptr) { lib::singleton().p_VSIFree(ptr); };
        unique_ptr<void, decltype(del)> buf(lib::singleton().p_VSIGetMemFileBuffer(file.c_str(), &len, true), del);
        row[i] = blob_t();
        blob_t& blob = ::boost::get<blob_t>(row[i]);
        const uint8_t* ptr(static_cast<const uint8_t*>(buf.get()));
        blob.assign(ptr, ptr + size_t(len));
      }
    }
    else
      row[i] = as_binary(geometry(m_tr.pixel_to_proj(box(point(0, 0), point(m_width, m_height)))));
  }
  return true;
} // rowset::

} } } // brig::gdal::detail

#endif // BRIG_GDAL_DETAIL_ROWSET_HPP
