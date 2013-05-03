// Andrew Naplavkov

#ifndef BRIG_GDAL_DETAIL_DATASET_HPP
#define BRIG_GDAL_DETAIL_DATASET_HPP

#include <brig/gdal/detail/lib.hpp>
#include <memory>
#include <stdexcept>

namespace brig { namespace gdal { namespace detail {

class dataset {
  std::shared_ptr<void> m_ds;
public:
  dataset(const std::string& ds);
  operator GDALDatasetH() const  { return m_ds.get(); }
}; // dataset

inline dataset::dataset(const std::string& ds_)
{
  using namespace std;

  if (lib::singleton().empty()) throw runtime_error("GDAL error");
  GDALDatasetH ds(lib::singleton().p_GDALOpen(ds_.c_str(), GA_ReadOnly));
  if (!ds) throw runtime_error("GDAL error");
  m_ds = shared_ptr<void>(ds, [](void* ptr){ lib::singleton().p_GDALClose(ptr); });
} // dataset::

} } } // brig::gdal::detail

#endif // BRIG_GDAL_DETAIL_DATASET_HPP
