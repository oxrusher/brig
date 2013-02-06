// Andrew Naplavkov

#ifndef BRIG_NUMERIC_CAST_HPP
#define BRIG_NUMERIC_CAST_HPP

#include <brig/detail/numeric_visitor.hpp>
#include <brig/variant.hpp>

namespace brig {

template <typename To>
bool numeric_cast(const variant& from, To& to)
{
  return ::boost::apply_visitor(detail::numeric_visitor<To>(to), from);
}

} // brig

#endif // BRIG_NUMERIC_CAST_HPP
