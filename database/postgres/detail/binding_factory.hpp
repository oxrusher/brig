// Andrew Naplavkov

#ifndef BRIG_DATABASE_POSTGRES_DETAIL_BINDING_FACTORY_HPP
#define BRIG_DATABASE_POSTGRES_DETAIL_BINDING_FACTORY_HPP

#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <brig/blob_t.hpp>
#include <brig/database/postgres/detail/binding.hpp>
#include <brig/database/postgres/detail/binding_blob.hpp>
#include <brig/database/postgres/detail/binding_impl.hpp>
#include <brig/database/postgres/detail/binding_null.hpp>
#include <brig/database/postgres/detail/binding_string.hpp>
#include <brig/database/postgres/detail/lib.hpp>
#include <brig/database/null_t.hpp>
#include <brig/database/variant.hpp>
#include <cstdint>
#include <string>

namespace brig { namespace database { namespace postgres { namespace detail {

struct binding_visitor : ::boost::static_visitor<binding*> {
  binding* operator()(const null_t&) const  { return new binding_null(); }
  binding* operator()(int16_t v) const  { return new binding_impl<int16_t, PG_TYPE_INT2>(v); }
  binding* operator()(int32_t v) const  { return new binding_impl<int32_t, PG_TYPE_INT4>(v); }
  binding* operator()(int64_t v) const  { return new binding_impl<int64_t, PG_TYPE_INT8>(v); }
  binding* operator()(float v) const  { return new binding_impl<float, PG_TYPE_FLOAT4>(v); }
  binding* operator()(double v) const  { return new binding_impl<double, PG_TYPE_FLOAT8>(v); }
  binding* operator()(const std::string& r) const  { return new binding_string(r); }
  binding* operator()(const blob_t& r) const  { return new binding_blob(r); }
}; // binding_visitor

inline binding* binding_factory(const variant& param)
{
  binding_visitor visitor;
  return ::boost::apply_visitor(visitor, param);
}

} } } } // brig::database::postgres::detail

#endif // BRIG_DATABASE_POSTGRES_DETAIL_BINDING_FACTORY_HPP
