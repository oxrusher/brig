// Andrew Naplavkov

#ifndef BRIG_DATABASE_POSTGRES_DETAIL_BINDING_IMPL_HPP
#define BRIG_DATABASE_POSTGRES_DETAIL_BINDING_IMPL_HPP

#include <boost/detail/endian.hpp>
#include <brig/database/postgres/detail/binding.hpp>
#include <brig/database/postgres/detail/lib.hpp>
#include <brig/detail/copy.hpp>
#include <cstdint>

namespace brig { namespace database { namespace postgres { namespace detail {

template <typename T, Oid TypeID>
class binding_impl : public binding {
  T m_val;
public:
  binding_impl(T val);
  Oid type() override  { return TypeID; }
  const char* value() override  { return (const char*)&m_val; }
  int length() override  { return sizeof(T); }
  int format() override  { return 1; }
}; // binding_impl

template <typename T, Oid TypeID>
binding_impl<T, TypeID>::binding_impl(T val)
{
#if defined BOOST_LITTLE_ENDIAN
  uint8_t *from((uint8_t*)&val), *to((uint8_t*)&m_val);
  brig::detail::reverse_copy<T>(from, to);
#elif defined BOOST_BIG_ENDIAN
  m_val = val;
#else
  #error byte order error
#endif
} // binding_impl::

} } } } // brig::database::postgres::detail

#endif // BRIG_DATABASE_POSTGRES_DETAIL_BINDING_IMPL_HPP
