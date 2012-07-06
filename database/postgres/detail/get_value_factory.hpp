// Andrew Naplavkov

#ifndef BRIG_DATABASE_POSTGRES_DETAIL_GET_VALUE_FACTORY_HPP
#define BRIG_DATABASE_POSTGRES_DETAIL_GET_VALUE_FACTORY_HPP

#include <brig/database/postgres/detail/get_value.hpp>
#include <brig/database/postgres/detail/get_value_blob.hpp>
#include <brig/database/postgres/detail/get_value_impl.hpp>
#include <brig/database/postgres/detail/get_value_string.hpp>
#include <brig/database/postgres/detail/lib.hpp>
#include <cstdint>
#include <stdexcept>

namespace brig { namespace database { namespace postgres { namespace detail {

inline get_value* get_value_factory(Oid type)
{
  switch (type)
  {
  default: throw std::runtime_error("Postgres type error");

  case PG_TYPE_BOOL: return new get_value_impl<int8_t>();

  case PG_TYPE_INT2: return new get_value_impl<int16_t>();
  case PG_TYPE_INT4: return new get_value_impl<int32_t>();
  case PG_TYPE_INT8: return new get_value_impl<int64_t>();
  case PG_TYPE_FLOAT4: return new get_value_impl<float>();
  case PG_TYPE_FLOAT8: return new get_value_impl<double>();

  case PG_TYPE_BPCHAR:
  case PG_TYPE_BPCHARARRAY:
  case PG_TYPE_NAME:
  case PG_TYPE_TEXT:
  case PG_TYPE_TEXTARRAY:
  case PG_TYPE_VARCHAR:
  case PG_TYPE_VARCHARARRAY: return new get_value_string();

  case PG_TYPE_BYTEA: return new get_value_blob();
  }
} // get_value_factory

} } } } // brig::database::postgres::detail

#endif // BRIG_DATABASE_POSTGRES_DETAIL_GET_VALUE_FACTORY_HPP
