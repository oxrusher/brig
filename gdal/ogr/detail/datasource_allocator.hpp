// Andrew Naplavkov

#ifndef BRIG_GDAL_OGR_DETAIL_DATASOURCE_ALLOCATOR_HPP
#define BRIG_GDAL_OGR_DETAIL_DATASOURCE_ALLOCATOR_HPP

#include <brig/gdal/ogr/detail/datasource.hpp>
#include <exception>
#include <string>

namespace brig { namespace gdal { namespace ogr { namespace detail {

class datasource_allocator {
  std::string m_ds, m_drv;
public:
  datasource_allocator(const std::string& ds, const std::string& drv = std::string()) : m_ds(ds), m_drv(drv)  { gdal::detail::lib::singleton(); }
  datasource allocate(bool writable);
}; // datasource_allocator

inline datasource datasource_allocator::allocate(bool writable)
{
  try  { return datasource(m_ds, writable); }
  catch (const std::exception&)  { if (m_drv.empty()) throw; }
  return datasource(m_drv, m_ds);
} // datasource_allocator::

} } } } // brig::gdal::ogr::detail

#endif // BRIG_GDAL_OGR_DETAIL_DATASOURCE_ALLOCATOR_HPP
