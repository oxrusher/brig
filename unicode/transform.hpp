// Andrew Naplavkov

#ifndef BRIG_UNICODE_TRANSFORM_HPP
#define BRIG_UNICODE_TRANSFORM_HPP

#include <brig/detail/back_insert_iterator.hpp>
#include <brig/unicode/detail/transform.hpp>
#include <cstdint>
#include <string>

namespace brig { namespace unicode {

template <typename OutputString, typename InputCodeUnit, typename Mapping>
OutputString transform(const InputCodeUnit* ptr, Mapping mapping)
{
  OutputString res;
  if (ptr != 0)
  {
    auto out_iter = brig::detail::back_inserter(res);
    detail::transform(ptr, out_iter, mapping);
  }
  return res;
}

template <typename OutputString, typename InputCodeUnit>
OutputString transform(const InputCodeUnit* ptr)
{
  if (sizeof(InputCodeUnit) == sizeof(typename OutputString::value_type)) return (const typename OutputString::value_type*)ptr;
  auto mapping([](uint32_t cp){ return cp; });
  return transform<OutputString, InputCodeUnit, decltype(mapping)>(ptr, mapping);
}

template <typename OutputString, typename InputCodeUnit, typename Mapping>
OutputString transform(const std::basic_string<InputCodeUnit>& str, Mapping mapping)
{
  OutputString res;
  auto in_ptr = str.c_str();
  auto out_iter = brig::detail::back_inserter(res);
  detail::transform(in_ptr, out_iter, mapping);
  return res;
}

template <typename OutputString, typename InputCodeUnit>
OutputString transform(const std::basic_string<InputCodeUnit>& str)
{
  if (sizeof(InputCodeUnit) == sizeof(typename OutputString::value_type)) return (const typename OutputString::value_type*)str.c_str();
  auto mapping([](uint32_t cp){ return cp; });
  return transform<OutputString, InputCodeUnit, decltype(mapping)>(str, mapping);
}

} } // brig::unicode

#endif // BRIG_UNICODE_TRANSFORM_HPP
