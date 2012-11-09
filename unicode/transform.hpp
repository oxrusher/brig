// Andrew Naplavkov

#ifndef BRIG_UNICODE_TRANSFORM_HPP
#define BRIG_UNICODE_TRANSFORM_HPP

#include <brig/unicode/detail/transform_impl.hpp>
#include <brig/unicode/detail/transformer.hpp>

namespace brig { namespace unicode {

template <typename OutputString, typename InputCodeUnit, typename Mapping>
OutputString transform(const InputCodeUnit* ptr, Mapping mapping)
{
  return detail::transform_impl<OutputString>(ptr, mapping);
}

template <typename OutputString, typename InputCodeUnit, typename Mapping>
OutputString transform(const std::basic_string<InputCodeUnit>& str, Mapping mapping)
{
  return detail::transform_impl<OutputString>(str.c_str(), mapping);
}

template <typename OutputString, typename InputCodeUnit>
OutputString transform(const InputCodeUnit* ptr)
{
  return detail::transformer<OutputString, InputCodeUnit, sizeof(InputCodeUnit) == sizeof(typename OutputString::value_type)>::apply(ptr);
}

template <typename OutputString, typename InputCodeUnit>
OutputString transform(const std::basic_string<InputCodeUnit>& str)
{
  return detail::transformer<OutputString, InputCodeUnit, sizeof(InputCodeUnit) == sizeof(typename OutputString::value_type)>::apply(str.c_str());
}

} } // brig::unicode

#endif // BRIG_UNICODE_TRANSFORM_HPP
