// Andrew Naplavkov

#ifndef BRIG_GDAL_DETAIL_DATASET_HPP
#define BRIG_GDAL_DETAIL_DATASET_HPP

#include <brig/gdal/detail/lib.hpp>
#include <stdexcept>

namespace brig { namespace gdal { namespace detail {

class dataset {
  GDALDatasetH m_hnd;
public:
  dataset(const std::string& ds);
  virtual ~dataset()  { lib::singleton().p_GDALClose(m_hnd); }
  operator GDALDatasetH() const  { return m_hnd; }
}; // dataset

inline dataset::dataset(const std::string& ds) : m_hnd(0)
{
  using namespace std;

  if (lib::singleton().empty()) throw runtime_error("GDAL error");
  m_hnd = lib::singleton().p_GDALOpen(ds.c_str(), GA_ReadOnly);
  if (!m_hnd) throw runtime_error("GDAL error");
} // dataset::

} } } // brig::gdal::detail

#endif // BRIG_GDAL_DETAIL_DATASET_HPP
