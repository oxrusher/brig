// Andrew Naplavkov

#ifndef BRIG_UNICODE_DETAIL_TRANSFORM_IMPL_HPP
#define BRIG_UNICODE_DETAIL_TRANSFORM_IMPL_HPP

#include <brig/detail/back_insert_iterator.hpp>
#include <brig/unicode/detail/utf.hpp>
#include <string>

namespace brig { namespace unicode { namespace detail {

template <typename OutputCodeUnit, typename InputCodeUnit, typename Mapping>
std::basic_string<OutputCodeUnit> transform_impl(const InputCodeUnit* ptr, Mapping mapping)
{
  std::basic_string<OutputCodeUnit> str;
  if (ptr == 0) return str;
  auto iter(brig::detail::back_inserter(str));
  while (true)
  {
    auto cp(utf<const InputCodeUnit*>::type::read_code_point(ptr));
    if (cp == 0) break;
    cp = mapping(cp);
    utf<decltype(iter)>::type::write_code_point(iter, cp);
  }
  return str;
}

} } } // brig::unicode::detail

#endif // BRIG_UNICODE_DETAIL_TRANSFORM_IMPL_HPP
