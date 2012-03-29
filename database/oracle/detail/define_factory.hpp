// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_DETAIL_DEFINE_FACTORY_HPP
#define BRIG_DATABASE_ORACLE_DETAIL_DEFINE_FACTORY_HPP

#include <brig/database/identifier.hpp>
#include <brig/database/oracle/detail/define.hpp>
#include <brig/database/oracle/detail/define_binary.hpp>
#include <brig/database/oracle/detail/define_blob.hpp>
#include <brig/database/oracle/detail/define_geometry.hpp>
#include <brig/database/oracle/detail/define_impl.hpp>
#include <brig/database/oracle/detail/define_string.hpp>
#include <brig/database/oracle/detail/get_charset_form.hpp>
#include <brig/database/oracle/detail/handles.hpp>
#include <brig/database/oracle/detail/lib.hpp>
#include <cstdint>
#include <stdexcept>

namespace brig { namespace database { namespace oracle { namespace detail {

inline define* define_factory(handles* hnd, size_t order, ub2 data_type, ub2 size, sb2 precision, sb1 scale, const identifier& lower_case_type)
{
  switch (data_type)
  {
  // binary
  case SQLT_BIN:
  case SQLT_VBI:
    return new define_binary(hnd, order, size);

  case SQLT_LBI:
  case SQLT_LVB:
  case SQLT_BFILE:
  case SQLT_BLOB:
    return new define_blob(hnd, order);

  // string
  case SQLT_DAT:
  case SQLT_DATE:
  case SQLT_ODT:
  case SQLT_TIMESTAMP:
  case SQLT_TIMESTAMP_TZ:
  case SQLT_TIMESTAMP_LTZ:

  case SQLT_AFC:
  case SQLT_AVC:
  case SQLT_CHR:
  case SQLT_STR:
  case SQLT_VCS:
  case SQLT_VST:
  case SQLT_LNG:
  case SQLT_LVC:
  case SQLT_CFILE:
  case SQLT_CLOB:
    return new define_string(hnd, order, size, get_charset_form(lower_case_type));

  // numeric
  case SQLT_INT:
  case SQLT_UIN:
    if (precision > 0)
    {
      if (precision <= 5) return new define_impl<int16_t, SQLT_INT>(hnd, order);
      else if (precision <= 10) return new define_impl<int32_t, SQLT_INT>(hnd, order);
    }
    return new define_impl<int64_t, SQLT_INT>(hnd, order);
  case SQLT_NUM:
    if (precision > 0)
    {
      if (scale == 0)
      {
        if (precision <= 5) return new define_impl<int16_t, SQLT_INT>(hnd, order);
        else if (precision <= 10) return new define_impl<int32_t, SQLT_INT>(hnd, order);
        else if (precision <= 19) return new define_impl<int64_t, SQLT_INT>(hnd, order);
      }
      else if (precision <= 6) return new define_impl<float, SQLT_FLT>(hnd, order);
    }
    return new define_impl<double, SQLT_FLT>(hnd, order);
  case SQLT_BFLOAT:
  case SQLT_IBFLOAT:
    return new define_impl<float, SQLT_FLT>(hnd, order);
  case SQLT_BDOUBLE:
  case SQLT_IBDOUBLE:
  case SQLT_PDN:
  case SQLT_VNU:
    return new define_impl<double, SQLT_FLT>(hnd, order);

  // named data type
  case SQLT_NTY:
    if ("mdsys" == lower_case_type.schema && "sdo_geometry" == lower_case_type.name) return new define_geometry(hnd, order);
    break;
  }

  throw std::runtime_error("OCI type error");
} // define_factory

} } } } // brig::database::oracle::detail

#endif // BRIG_DATABASE_ORACLE_DETAIL_DEFINE_FACTORY_HPP
