// Andrew Naplavkov

#ifndef BRIG_UNICODE_DETAIL_TRANSFORMER_HPP
#define BRIG_UNICODE_DETAIL_TRANSFORMER_HPP

#include <brig/unicode/detail/transform_impl.hpp>

namespace brig { namespace unicode { namespace detail {

template <typename OutputString, typename InputCodeUnit, bool Trivial>
struct transformer;

template <typename OutputString, typename InputCodeUnit>
struct transformer<OutputString, InputCodeUnit, true>
{
  static OutputString apply(const InputCodeUnit* ptr)  { return (const typename OutputString::value_type*)ptr; }
};

template <typename OutputString, typename InputCodeUnit>
struct transformer<OutputString, InputCodeUnit, false>
{
  static OutputString apply(const InputCodeUnit* ptr)  { return transform_impl<OutputString>(ptr, [](uint32_t cp){ return cp; }); }
};

} } } // brig::unicode::detail

#endif // BRIG_UNICODE_DETAIL_TRANSFORMER_HPP
