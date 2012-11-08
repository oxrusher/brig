// Andrew Naplavkov

#ifndef BRIG_DATABASE_CUBRID_DETAIL_GET_DATA_BLOB_HPP
#define BRIG_DATABASE_CUBRID_DETAIL_GET_DATA_BLOB_HPP

#include <brig/blob_t.hpp>
#include <brig/database/cubrid/detail/get_data.hpp>
#include <brig/database/cubrid/detail/lib.hpp>
#include <brig/database/variant.hpp>
#include <cstring>

namespace brig { namespace database { namespace cubrid { namespace detail {

struct get_data_blob : get_data {
  int operator()(int req, size_t col, variant& var) override;
}; // get_data_blob

inline int get_data_blob::operator()(int req, size_t col, variant& var)
{
  T_CCI_BIT data;
  int ind(-1);
  const int r(lib::singleton().p_cci_get_data(req, col + 1, CCI_A_TYPE_BIT, &data, &ind));
  if (lib::error(r) || ind < 0)
    var = null_t();
  else
  {
    var = blob_t();
    blob_t& blob(::boost::get<blob_t>(var));
    blob.resize(data.size);
    if (!blob.empty()) memcpy(blob.data(), data.buf, data.size);
  }
  return r;
} // get_data_blob::

} } } } // brig::database::cubrid::detail

#endif // BRIG_DATABASE_CUBRID_DETAIL_GET_DATA_BLOB_HPP
