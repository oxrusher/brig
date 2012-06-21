// Andrew Naplavkov

#ifndef BRIG_DATABASE_CUBRID_DETAIL_GET_DATA_FACTORY_HPP
#define BRIG_DATABASE_CUBRID_DETAIL_GET_DATA_FACTORY_HPP

#include <brig/database/cubrid/detail/get_data.hpp>
#include <brig/database/cubrid/detail/get_data_blob.hpp>
#include <brig/database/cubrid/detail/get_data_impl.hpp>
#include <brig/database/cubrid/detail/get_data_string.hpp>
#include <brig/database/cubrid/detail/lib.hpp>
#include <cstdint>
#include <stdexcept>

namespace brig { namespace database { namespace cubrid { namespace detail {

inline get_data* get_data_factory(T_CCI_U_TYPE type)
{
  switch (type)
  {
  // numeric
  case CCI_U_TYPE_SHORT:
  case CCI_U_TYPE_INT: return new get_data_impl<int32_t, CCI_A_TYPE_INT>();
  case CCI_U_TYPE_BIGINT: return new get_data_impl<int64_t, CCI_A_TYPE_BIGINT>();

  case CCI_U_TYPE_FLOAT: return new get_data_impl<float, CCI_A_TYPE_FLOAT>();
  case CCI_U_TYPE_NUMERIC:
  case CCI_U_TYPE_MONETARY:
  case CCI_U_TYPE_DOUBLE: return new get_data_impl<double, CCI_A_TYPE_DOUBLE>();

  // string
  case CCI_U_TYPE_DATE:
  case CCI_U_TYPE_DATETIME:
  case CCI_U_TYPE_TIME:
  case CCI_U_TYPE_TIMESTAMP:

  case CCI_U_TYPE_CLOB:
  case CCI_U_TYPE_CHAR:
  case CCI_U_TYPE_NCHAR:
  case CCI_U_TYPE_STRING:
  case CCI_U_TYPE_VARNCHAR: return new get_data_string();

  // blob
  case CCI_U_TYPE_BIT:
  case CCI_U_TYPE_BLOB:
  case CCI_U_TYPE_VARBIT: return new get_data_blob();

  // CCI_U_TYPE_UNKNOWN
  default: throw std::runtime_error("CUBRID type error");
  }
} // get_data_factory

} } } } // brig::database::cubrid::detail

#endif // BRIG_DATABASE_CUBRID_DETAIL_GET_DATA_FACTORY_HPP
