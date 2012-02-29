// Andrew Naplavkov

#ifndef BRIG_UNICODE_DETAIL_TRANSFORM_HPP
#define BRIG_UNICODE_DETAIL_TRANSFORM_HPP

#include <brig/unicode/detail/utf.hpp>
#include <cstdint>

namespace brig { namespace unicode { namespace detail {

template <typename InputIterator, typename OutputIterator, typename Mapping>
void transform(InputIterator& in_iter, OutputIterator& out_iter, Mapping mapping)
{
  while (true)
  {
    uint32_t cp(detail::utf<InputIterator>::type::read_code_point(in_iter));
    if (cp == 0) break;
    cp = mapping(cp);
    detail::utf<OutputIterator>::type::write_code_point(out_iter, cp);
  }
}

} } } // brig::unicode::detail

#endif // BRIG_UNICODE_DETAIL_TRANSFORM_HPP
