// Andrew Naplavkov

#ifndef BRIG_UNICODE_DETAIL_UTF32_HPP
#define BRIG_UNICODE_DETAIL_UTF32_HPP

#include <cstdint>
#include <iterator>

namespace brig { namespace unicode { namespace detail {

struct utf32
{
  template <typename InputIterator>
  static uint32_t read_code_point(InputIterator& in_itr)
  {
    static_assert(sizeof(typename std::iterator_traits<InputIterator>::value_type) == sizeof(uint32_t), "UTF-32 error");
    const uint32_t cp(static_cast<uint32_t>(*in_itr)); ++in_itr;
    return cp;
  }

  template <typename OutputIterator>
  static void write_code_point(OutputIterator& out_itr, const uint32_t cp)
  {
    static_assert(sizeof(typename std::iterator_traits<OutputIterator>::value_type) == sizeof(uint32_t), "UTF-32 error");
    *out_itr = cp; ++out_itr;
  }
}; // utf32

} } } // brig::unicode::detail

#endif // BRIG_UNICODE_DETAIL_UTF32_HPP
