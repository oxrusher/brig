// Andrew Naplavkov

#ifndef BRIG_GDAL_DETAIL_DATASET_ALLOCATOR_HPP
#define BRIG_GDAL_DETAIL_DATASET_ALLOCATOR_HPP

#include <boost/utility.hpp>
#include <brig/gdal/detail/dataset.hpp>
#include <exception>
#include <string>

namespace brig { namespace gdal { namespace detail {

class dataset_allocator : ::boost::noncopyable {
  std::string m_ds;
public:
  dataset_allocator(const std::string& ds) : m_ds(ds)  { lib::singleton(); }
  dataset* allocate()  { return new dataset(m_ds); }
}; // dataset_allocator

} } } // brig::gdal::detail

#endif // BRIG_GDAL_DETAIL_DATASET_ALLOCATOR_HPP
