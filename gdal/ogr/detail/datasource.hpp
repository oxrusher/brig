// Andrew Naplavkov

#ifndef BRIG_GDAL_OGR_DETAIL_DATASOURCE_HPP
#define BRIG_GDAL_OGR_DETAIL_DATASOURCE_HPP

#include <brig/gdal/detail/lib.hpp>
#include <stdexcept>

namespace brig { namespace gdal { namespace ogr { namespace detail {

class datasource {
  OGRDataSourceH m_hnd;
public:
  datasource(const std::string& ds, bool writable);
  datasource(const std::string& drv, const std::string& ds);
  virtual ~datasource()  { gdal::detail::lib::singleton().p_OGR_DS_Destroy(m_hnd); }
  operator OGRDataSourceH() const  { return m_hnd; }
}; // datasource

inline datasource::datasource(const std::string& ds, bool writable) : m_hnd(0)
{
  using namespace std;
  using namespace gdal::detail;

  if (lib::singleton().empty()) throw runtime_error("OGR error");
  m_hnd = lib::singleton().p_OGROpen(ds.c_str(), writable, 0);
  if (!m_hnd) throw runtime_error("OGR error");
}

inline datasource::datasource(const std::string& drv_, const std::string& ds_) : m_hnd(0)
{
  using namespace std;
  using namespace gdal::detail;

  if (lib::singleton().empty()) throw runtime_error("OGR error");
  OGRSFDriverH drv(lib::singleton().p_OGRGetDriverByName(drv_.c_str()));
  if (!drv) throw runtime_error("OGR error");
  m_hnd = lib::singleton().p_OGR_Dr_CreateDataSource(drv, ds_.c_str(), 0);
  if (!m_hnd) throw runtime_error("OGR error");
} // datasource::

} } } } // brig::gdal::ogr::detail

#endif // BRIG_GDAL_OGR_DETAIL_DATASOURCE_HPP
