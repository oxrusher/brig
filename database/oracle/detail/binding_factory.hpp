// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_DETAIL_BINDING_FACTORY_HPP
#define BRIG_DATABASE_ORACLE_DETAIL_BINDING_FACTORY_HPP

#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <brig/blob_t.hpp>
#include <brig/database/column_definition.hpp>
#include <brig/database/global.hpp>
#include <brig/database/oracle/detail/binding.hpp>
#include <brig/database/oracle/detail/binding_blob.hpp>
#include <brig/database/oracle/detail/binding_geometry.hpp>
#include <brig/database/oracle/detail/binding_impl.hpp>
#include <brig/database/oracle/detail/binding_string.hpp>
#include <brig/database/oracle/detail/get_charset_form.hpp>
#include <brig/database/oracle/detail/handles.hpp>
#include <brig/database/variant.hpp>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace brig { namespace database { namespace oracle { namespace detail {

struct binding_visitor : ::boost::static_visitor<binding*> {
  handles* hnd;
  size_t i;
  const column_definition* col;
  binding_visitor(handles* hnd_, size_t i_, const column_definition* col_) : hnd(hnd_), i(i_), col(col_)  {}
  binding* operator()(const null_t&) const;
  binding* operator()(int16_t v) const  { return new binding_impl<int16_t, SQLT_INT>(hnd, i, v); }
  binding* operator()(int32_t v) const  { return new binding_impl<int32_t, SQLT_INT>(hnd, i, v); }
  binding* operator()(int64_t v) const  { return new binding_impl<int64_t, SQLT_INT>(hnd, i, v); }
  binding* operator()(float v) const  { return new binding_impl<float, SQLT_FLT>(hnd, i, v); }
  binding* operator()(double v) const  { return new binding_impl<double, SQLT_FLT>(hnd, i, v); }
  binding* operator()(const std::string& r) const  { return new binding_string(hnd, i, r, col? get_charset_form(col->dbms_type_lcase): SQLCS_NCHAR); }
  binding* operator()(const blob_t&) const;
}; // binding_visitor

inline binding* binding_visitor::operator()(const null_t&) const
{
  switch (col? col->type: VoidColumn)
  {
    default: throw std::runtime_error("OCI type error");
    case Blob: return new binding_blob(hnd, i, 0, 0);
    case Double: return new binding_impl<double, SQLT_FLT>(hnd, i);
    case Geometry: return new binding_geometry(hnd, i, blob_t(), col->srid);
    case Integer: return new binding_impl<int64_t, SQLT_INT>(hnd, i);
    case String: return new binding_string(hnd, i, std::string(), get_charset_form(col->dbms_type_lcase));
  };
}

inline binding* binding_visitor::operator()(const blob_t& r) const
{
  if (col && Geometry == col->type)
    return new binding_geometry(hnd, i, r, col->srid);
  else
    return new binding_blob(hnd, i, (void*)r.data(), ub4(r.size()));
} // binding_visitor::

inline binding* binding_factory(handles* hnd, size_t param, const variant& var, const column_definition* param_col)
{
  return ::boost::apply_visitor(binding_visitor(hnd, param + 1, param_col), var);
}

} } } } // brig::database::oracle::detail

#endif // BRIG_DATABASE_ORACLE_DETAIL_BINDING_FACTORY_HPP
