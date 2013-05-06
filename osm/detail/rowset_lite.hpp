// Andrew Naplavkov

#ifndef BRIG_OSM_DETAIL_ROWSET_LITE_HPP
#define BRIG_OSM_DETAIL_ROWSET_LITE_HPP

#include <brig/boost/geometry.hpp>
#include <brig/boost/as_binary.hpp>
#include <brig/global.hpp>
#include <brig/osm/detail/tile.hpp>
#include <brig/osm/detail/tiles.hpp>
#include <brig/rowset.hpp>
#include <memory>
#include <string>
#include <vector>

namespace brig { namespace osm { namespace detail {

class rowset_lite : public brig::rowset {
  const size_t m_cols;
  int m_rows;
  tiles m_iter;
public:
  rowset_lite(size_t cols, int zoom, const brig::boost::box& env, int rows) : m_cols(cols), m_rows(rows), m_iter(zoom, env)  {}
  std::vector<std::string> columns() override;
  bool fetch(std::vector<variant>& row) override;
}; // rowset

inline std::vector<std::string> rowset_lite::columns()
{
  std::vector<std::string> cols;
  for (size_t i(0); i < m_cols; ++i)
    cols.push_back(ColumnNameWkb);
  return cols;
}

inline bool rowset_lite::fetch(std::vector<variant>& row)
{
  if (m_rows == 0) return false;
  tile tl(0, 0, 0);
  if (!m_iter.fetch(tl)) return false;
  if (m_rows > 0) --m_rows;

  const blob_t blob(brig::boost::as_binary(tl.get_box()));
  row.resize(m_cols);
  for (size_t i(0); i < m_cols; ++i)
    row[i] = blob;
  return true;
} // rowset::

} } } // brig::osm::detail

#endif // BRIG_OSM_DETAIL_ROWSET_LITE_HPP
