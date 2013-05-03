// Andrew Naplavkov

#ifndef BRIG_GDAL_OGR_DETAIL_DATASOURCE_HPP
#define BRIG_GDAL_OGR_DETAIL_DATASOURCE_HPP

#include <brig/gdal/detail/lib.hpp>
#include <memory>
#include <stdexcept>

namespace brig { namespace gdal { namespace ogr { namespace detail {

class datasource {
  std::shared_ptr<void> m_ds;
public:
  datasource(const std::string& ds, bool writable);
  datasource(const std::string& drv, const std::string& ds);
  operator OGRDataSourceH() const  { return m_ds.get(); }
}; // datasource

inline datasource::datasource(const std::string& ds_, bool writable)
{
  using namespace std;
  using namespace gdal::detail;

  if (lib::singleton().empty()) throw runtime_error("OGR error");
  OGRDataSourceH ds(lib::singleton().p_OGROpen(ds_.c_str(), writable, 0));
  if (!ds) throw runtime_error("OGR error");
  m_ds = shared_ptr<void>(ds, [](void* ptr){ lib::singleton().p_OGR_DS_Destroy(ptr); });
}

inline datasource::datasource(const std::string& drv_, const std::string& ds_)
{
  using namespace std;
  using namespace gdal::detail;

  if (lib::singleton().empty()) throw runtime_error("OGR error");
  OGRSFDriverH drv(lib::singleton().p_OGRGetDriverByName(drv_.c_str()));
  if (!drv) throw runtime_error("OGR error");
  OGRDataSourceH ds(lib::singleton().p_OGR_Dr_CreateDataSource(drv, ds_.c_str(), 0));
  if (!ds) throw runtime_error("OGR error");
  m_ds = shared_ptr<void>(ds, [](void* ptr){ lib::singleton().p_OGR_DS_Destroy(ptr); });
} // datasource::

} } } } // brig::gdal::ogr::detail

#endif // BRIG_GDAL_OGR_DETAIL_DATASOURCE_HPP
