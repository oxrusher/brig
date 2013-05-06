// Andrew Naplavkov

#ifndef BRIG_DATABASE_CUBRID_DETAIL_BINDING_HPP
#define BRIG_DATABASE_CUBRID_DETAIL_BINDING_HPP

#include <brig/column_def.hpp>
#include <brig/database/cubrid/detail/lib.hpp>
#include <brig/variant.hpp>

namespace brig { namespace database { namespace cubrid { namespace detail {

struct binding_visitor : ::boost::static_visitor<int> {
  int req;
  int i;
  T_CCI_U_TYPE type;

  int operator()(const null_t&) const  { return lib::singleton().p_cci_bind_param(req, i, CCI_A_TYPE_STR, 0, CCI_U_TYPE_NULL, 0); }
  int operator()(int16_t v) const  { int32_t v32(v); return lib::singleton().p_cci_bind_param(req, i, CCI_A_TYPE_INT, &v32, type != CCI_U_TYPE_NULL? type: CCI_U_TYPE_SHORT, 0); }
  int operator()(int32_t v) const  { return lib::singleton().p_cci_bind_param(req, i, CCI_A_TYPE_INT, &v, type != CCI_U_TYPE_NULL? type: CCI_U_TYPE_INT, 0); }
  int operator()(int64_t v) const  { return lib::singleton().p_cci_bind_param(req, i, CCI_A_TYPE_BIGINT, &v, type != CCI_U_TYPE_NULL? type: CCI_U_TYPE_BIGINT, 0); }
  int operator()(float v) const  { return lib::singleton().p_cci_bind_param(req, i, CCI_A_TYPE_FLOAT, &v, type != CCI_U_TYPE_NULL? type: CCI_U_TYPE_FLOAT, 0); }
  int operator()(double v) const  { return lib::singleton().p_cci_bind_param(req, i, CCI_A_TYPE_DOUBLE, &v, type != CCI_U_TYPE_NULL? type: CCI_U_TYPE_DOUBLE, 0); }
  int operator()(const blob_t& r) const;
  int operator()(const std::string& r) const  { return lib::singleton().p_cci_bind_param(req, i, CCI_A_TYPE_STR, (void*)r.c_str(), type != CCI_U_TYPE_NULL? type: CCI_U_TYPE_STRING, CCI_BIND_PTR); }
}; // binding_visitor

inline int binding_visitor::operator()(const blob_t& r) const
{
  T_CCI_BIT bit;
  bit.buf = (char*)r.data();
  bit.size = int(r.size());
  return lib::singleton().p_cci_bind_param(req, i, CCI_A_TYPE_BIT, &bit, type != CCI_U_TYPE_NULL? type: CCI_U_TYPE_BIT, 0);
}

inline int bind(int req, size_t order, const column_def& param)
{
  binding_visitor visitor;
  visitor.req = req;
  visitor.i = int(order + 1);
  visitor.type = CCI_U_TYPE_NULL;

  switch (param.type)
  {
  case column_type::Void: break;
  case column_type::Blob:
  case column_type::Geometry: visitor.type = CCI_U_TYPE_VARBIT; break;
  case column_type::Double: visitor.type = CCI_U_TYPE_DOUBLE; break;
  case column_type::Integer: visitor.type = CCI_U_TYPE_BIGINT; break;
  case column_type::String: visitor.type
    = (param.type_lcase.name.find("nchar") != std::string::npos || param.type_lcase.name.find("national") != std::string::npos)
    ? CCI_U_TYPE_VARNCHAR
    : CCI_U_TYPE_STRING;
    break;
  }

  return ::boost::apply_visitor(visitor, param.query_value);
}

} } } } // brig::database::cubrid::detail

#endif // BRIG_DATABASE_CUBRID_DETAIL_BINDING_HPP
