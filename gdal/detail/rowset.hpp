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
#include <brig/table_def.hpp>
#include <cmath>
#include <cstdint>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <vector>

namespace brig { namespace gdal { namespace detail {

class rowset : public brig::rowset
{
  static const int TileSize = 256;

  std::unique_ptr<dataset> m_ds;
  std::vector<bool> m_cols;
  int m_rows, m_width, m_height, m_tiles;
  transform m_tr;
  brig::boost::box m_block;

public:
  rowset(dataset_allocator* allocator, const table_def& tbl);
  std::vector<std::string> columns() override;
  bool fetch(std::vector<variant>& row) override;
}; // rowset

inline rowset::rowset(dataset_allocator* allocator, const table_def& tbl) : m_ds(allocator->allocate())
{
  using namespace std;
  using namespace brig::boost;

  vector<column_def> col_defs = tbl.query_columns.empty()? tbl.columns: brig::detail::get_columns(tbl.columns, tbl.query_columns);
  for (auto iter(begin(col_defs)); iter != end(col_defs); ++iter)
  {
    if (iter->name.compare(PNG()) == 0)
      m_cols.push_back(true);
    else if (iter->name.compare(WKB()) == 0)
      m_cols.push_back(false);
    else
      throw runtime_error("GDAL error");
  }
  m_rows = tbl.query_rows;

  m_width = lib::singleton().p_GDALGetRasterXSize(*m_ds);
  m_height = lib::singleton().p_GDALGetRasterYSize(*m_ds);
  m_tiles = int(ceil(double(m_width) / double(TileSize)) * ceil(double(m_height) / double(TileSize)));
  lib::check(lib::singleton().p_GDALGetGeoTransform(*m_ds, m_tr.coef));

  auto geom_col(tbl[WKB()]);
  if (typeid(null_t) == geom_col->query_value.type())
    m_block = box(point(0, 0), point(m_width, m_height));
  else if (!::boost::geometry::intersection
      ( box(point(0, 0), point(m_width, m_height))
      , envelope(m_tr.proj_to_pixel(envelope(geom_from_wkb(::boost::get<blob_t>(geom_col->query_value)))))
      , m_block
      ))
    m_tiles = 0;
}

inline std::vector<std::string> rowset::columns()
{
  std::vector<std::string> cols;
  for (size_t i(0); i < m_cols.size(); ++i)
  {
    if (m_cols[i]) cols.push_back(PNG());
    else cols.push_back(WKB());
  }
  return cols;
}

inline bool rowset::fetch(std::vector<variant>& row)
{
  using namespace std;
  using namespace brig::boost;

  if (m_rows == 0) return false;
  box block;
  const int isize(int(ceil(double(m_width) / double(TileSize))));
  while (true)
  {
    if (m_tiles <= 0) return false;
    --m_tiles;
    const int i(m_tiles % isize);
    const int j(m_tiles / isize);
    block = box(point(i * TileSize, j * TileSize), point(min<>((i + 1) * TileSize, m_width), min<>((j + 1) * TileSize, m_height)));
    if (::boost::geometry::intersects(block, m_block)) break;
  }
  if (m_rows > 0) --m_rows;

  const size_t count(m_cols.size());
  row.resize(count);
  for (size_t i(0); i < count; ++i)
  {
    if (m_cols[i])
    {
      const int bands(lib::singleton().p_GDALGetRasterCount(*m_ds));
      for (int band(1); band <= bands; ++band)
      {
        const int xoff(int(block.min_corner().get<0>()));
        const int yoff(int(block.min_corner().get<1>()));
        const int xsize(int(block.max_corner().get<0>() - block.min_corner().get<0>()));
        const int ysize(int(block.max_corner().get<1>() - block.min_corner().get<1>()));
        blob_t buf;
        buf.resize(xsize * ysize * sizeof(uint32_t));
        lib::check(lib::singleton().p_GDALRasterIO(lib::singleton().p_GDALGetRasterBand(*m_ds, band), GF_Read, xoff, yoff, xsize, ysize, buf.data(), xsize, ysize, GDT_UInt32, 0, 0));
      }
      row[i] = null_t(); // todo: libpng
    }
    else
      row[i] = as_binary(geometry(m_tr.pixel_to_proj(block)));
  }
  return true;
} // rowset::

} } } // brig::gdal::detail

#endif // BRIG_GDAL_DETAIL_ROWSET_HPP
