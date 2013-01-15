// Andrew Naplavkov

#ifndef BRIG_GDAL_DETAIL_DATASOURCE_ALLOCATOR_HPP
#define BRIG_GDAL_DETAIL_DATASOURCE_ALLOCATOR_HPP

#include <boost/utility.hpp>
#include <brig/gdal/detail/datasource.hpp>
#include <exception>

namespace brig { namespace gdal { namespace detail {

class datasource_allocator : ::boost::noncopyable {
  std::string m_ds, m_drv;
public:
  datasource_allocator(const std::string& ds, const std::string& drv = std::string()) : m_ds(ds), m_drv(drv)  { lib::singleton(); }
  datasource* allocate(bool writable);
}; // datasource_allocator

inline datasource* datasource_allocator::allocate(bool writable)
{
  try  { return new datasource(m_ds, writable); }
  catch (const std::exception&)  { if (m_drv.empty()) throw; }
  return new datasource(m_drv, m_ds);
}

} } } // brig::gdal::detail

#endif // BRIG_GDAL_DETAIL_DATASOURCE_ALLOCATOR_HPP
