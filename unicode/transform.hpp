// Andrew Naplavkov

#ifndef BRIG_UNICODE_TRANSFORM_HPP
#define BRIG_UNICODE_TRANSFORM_HPP

#include <brig/unicode/detail/transform_impl.hpp>
#include <brig/unicode/detail/transformer.hpp>

namespace brig { namespace unicode {

template <typename OutputCodeUnit, typename InputCodeUnit, typename Mapping>
std::basic_string<OutputCodeUnit> transform(const InputCodeUnit* ptr, Mapping mapping)
{
  return detail::transform_impl<OutputCodeUnit>(ptr, mapping);
}

template <typename OutputCodeUnit, typename InputCodeUnit, typename Mapping>
std::basic_string<OutputCodeUnit> transform(const std::basic_string<InputCodeUnit>& str, Mapping mapping)
{
  return detail::transform_impl<OutputCodeUnit>(str.c_str(), mapping);
}

template <typename OutputCodeUnit, typename InputCodeUnit>
std::basic_string<OutputCodeUnit> transform(const InputCodeUnit* ptr)
{
  return detail::transformer<OutputCodeUnit, InputCodeUnit, sizeof(InputCodeUnit) == sizeof(OutputCodeUnit)>::apply(ptr);
}

template <typename OutputCodeUnit, typename InputCodeUnit>
std::basic_string<OutputCodeUnit> transform(const std::basic_string<InputCodeUnit>& str)
{
  return detail::transformer<OutputCodeUnit, InputCodeUnit, sizeof(InputCodeUnit) == sizeof(OutputCodeUnit)>::apply(str.c_str());
}

} } // brig::unicode

#endif // BRIG_UNICODE_TRANSFORM_HPP
