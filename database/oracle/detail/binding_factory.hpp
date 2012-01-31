// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_DETAIL_BINDING_FACTORY_HPP
#define BRIG_DATABASE_ORACLE_DETAIL_BINDING_FACTORY_HPP

#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <brig/blob_t.hpp>
#include <brig/database/column_detail.hpp>
#include <brig/database/dbms.hpp>
#include <brig/database/detail/get_type_identifier.hpp>
#include <brig/database/oracle/detail/binding.hpp>
#include <brig/database/oracle/detail/binding_datetime.hpp>
#include <brig/database/oracle/detail/binding_geometry.hpp>
#include <brig/database/oracle/detail/binding_impl.hpp>
#include <brig/database/oracle/detail/binding_string.hpp>
#include <brig/database/oracle/detail/get_charset_form.hpp>
#include <brig/database/oracle/detail/handles.hpp>
#include <brig/database/type_identifier.hpp>
#include <brig/database/variant.hpp>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace brig { namespace database { namespace oracle { namespace detail {

struct binding_visitor : boost::static_visitor<binding*> {
  handles* hnd;
  size_t i;
  const column_detail* col;
  binding_visitor(handles* hnd_, size_t i_, const column_detail* col_) : hnd(hnd_), i(i_), col(col_)  {}
  binding* operator()(const null_t&) const;
  binding* operator()(int16_t v) const  { return new binding_impl<int16_t, SQLT_INT>(hnd, i, v); }
  binding* operator()(int32_t v) const  { return new binding_impl<int32_t, SQLT_INT>(hnd, i, v); }
  binding* operator()(int64_t v) const  { return new binding_impl<int64_t, SQLT_INT>(hnd, i, v); }
  binding* operator()(float v) const  { return new binding_impl<float, SQLT_FLT>(hnd, i, v); }
  binding* operator()(double v) const  { return new binding_impl<double, SQLT_FLT>(hnd, i, v); }
  binding* operator()(const boost::gregorian::date& r) const  { return new binding_datetime(hnd, i, r); }
  binding* operator()(const boost::posix_time::ptime& r) const  { return new binding_datetime(hnd, i, r); }
  binding* operator()(const std::string& r) const  { return new binding_string(hnd, i, r, col? get_charset_form(col->type_schema, col->type_name): SQLCS_NCHAR); }
  binding* operator()(const blob_t& r) const  { return new binding_geometry(hnd, i, r, col? col->srid: -1); }
}; // binding_visitor

inline binding* binding_visitor::operator()(const null_t&) const
{
  switch (col? brig::database::detail::get_type_identifier(Oracle, *col): UnknownType)
  {
    default: throw std::runtime_error("unsupported OCI parameter");
    case Date:
    case DateTime: return new binding_datetime(hnd, i);
    case Double: return new binding_impl<double, SQLT_FLT>(hnd, i);
    case Geometry: return new binding_geometry(hnd, i, blob_t(), col->srid);
    case Integer: return new binding_impl<int64_t, SQLT_INT>(hnd, i);
    case String: return new binding_string(hnd, i, std::string(), get_charset_form(col->type_schema, col->type_name));
  };
} // binding_visitor::

inline binding* binding_factory(handles* hnd, size_t param, const variant& var, const column_detail* param_col)
{
  return boost::apply_visitor(binding_visitor(hnd, param + 1, param_col), var);
}

} } } } // brig::database::oracle::detail

#endif // BRIG_DATABASE_ORACLE_DETAIL_BINDING_FACTORY_HPP
