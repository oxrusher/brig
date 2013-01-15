// Andrew Naplavkov

#ifndef BRIG_UNICODE_DETAIL_TRANSFORMER_HPP
#define BRIG_UNICODE_DETAIL_TRANSFORMER_HPP

#include <brig/unicode/detail/transform_impl.hpp>

namespace brig { namespace unicode { namespace detail {

template <typename OutputCodeUnit, typename InputCodeUnit, bool Trivial>
struct transformer;

template <typename OutputCodeUnit, typename InputCodeUnit>
struct transformer<OutputCodeUnit, InputCodeUnit, true>
{
  static std::basic_string<OutputCodeUnit> apply(const InputCodeUnit* ptr)  { return (const OutputCodeUnit*)ptr; }
};

template <typename OutputCodeUnit, typename InputCodeUnit>
struct transformer<OutputCodeUnit, InputCodeUnit, false>
{
  static std::basic_string<OutputCodeUnit> apply(const InputCodeUnit* ptr)  { return transform_impl<OutputCodeUnit>(ptr, [](uint32_t cp){ return cp; }); }
};

} } } // brig::unicode::detail

#endif // BRIG_UNICODE_DETAIL_TRANSFORMER_HPP
