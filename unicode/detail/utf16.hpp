// Andrew Naplavkov

#ifndef BRIG_UNICODE_DETAIL_UTF16_HPP
#define BRIG_UNICODE_DETAIL_UTF16_HPP

#include <cstdint>
#include <iterator>
#include <stdexcept>

namespace brig { namespace unicode { namespace detail {

class utf16 {
  static bool high(const uint16_t cu)  { return 0xd800 <= cu && cu <= 0xdbff; }
  static bool low(const uint16_t cu)  { return 0xdc00 <= cu && cu <= 0xdfff; }

public:
  template <typename InputIterator>
  static uint32_t get_code_point(InputIterator& in_iter)
  {
    static_assert(sizeof(typename std::iterator_traits<InputIterator>::value_type) == sizeof(uint16_t), "UTF-16 error");
    const uint16_t cu1(static_cast<uint16_t>(*in_iter)); ++in_iter;
    if (high(cu1))
    {
      const uint16_t cu2(static_cast<uint16_t>(*in_iter)); ++in_iter;
      if (low(cu2)) return (static_cast<uint32_t>(cu1) << 10) + cu2 + (0x10000 - (0xd800 << 10) - 0xdc00);
    }
    else if (!low(cu1)) return static_cast<uint32_t>(cu1);
    throw std::runtime_error("UTF-16 error");
  }

  template <typename OutputIterator>
  static void set_code_point(OutputIterator& out_iter, const uint32_t cp)
  {
    static_assert(sizeof(typename std::iterator_traits<OutputIterator>::value_type) == sizeof(uint16_t), "UTF-16 error");
    if (cp < 0xffff)
    {
      if (low(cp)) throw std::runtime_error("UTF-16 error");
      *out_iter = static_cast<uint16_t>(cp); ++out_iter;
    }
    else if (cp < 0x10ffff)
    {
      *out_iter = static_cast<uint16_t>(0xd800 - (0x10000 >> 10) + (cp >> 10)); ++out_iter;
      *out_iter = static_cast<uint16_t>(0xdc00 + (cp & 0x3ff)); ++out_iter;
    }
    else throw std::runtime_error("UTF-16 error");
  }
}; // utf16

} } } // brig::unicode::detail

#endif // BRIG_UNICODE_DETAIL_UTF16_HPP
