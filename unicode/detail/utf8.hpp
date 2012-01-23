// Andrew Naplavkov

#ifndef BRIG_UNICODE_DETAIL_UTF8_HPP
#define BRIG_UNICODE_DETAIL_UTF8_HPP

#include <cstdint>
#include <iterator>
#include <stdexcept>

namespace brig { namespace unicode { namespace detail {

class utf8 {
  static void check_continuation(uint8_t cu)
  {
    if ((cu >> 6) != 0x2) throw std::runtime_error("UTF-8 error");
  }

public:
  template <typename InputIterator>
  static uint32_t get_code_point(InputIterator& in_iter)
  {
    static_assert(sizeof(typename std::iterator_traits<InputIterator>::value_type) == sizeof(uint8_t), "UTF-8 error");
    const uint8_t cu1(static_cast<uint8_t>(*in_iter)); ++in_iter;
    if (cu1 < 0x80) return static_cast<uint32_t>(cu1);
    else if ((cu1 >> 5) == 0x6)
    {
      const uint8_t cu2(static_cast<uint8_t>(*in_iter)); ++in_iter; check_continuation(cu2);
      return ((static_cast<uint32_t>(cu1) << 6) & 0x7ff) + (cu2 & 0x3f);
    }
    else if ((cu1 >> 4) == 0xe)
    {
      const uint8_t cu2(static_cast<uint8_t>(*in_iter)); ++in_iter; check_continuation(cu2);
      const uint8_t cu3(static_cast<uint8_t>(*in_iter)); ++in_iter; check_continuation(cu3);
      return ((static_cast<uint32_t>(cu1) << 12) & 0xffff) + ((static_cast<uint32_t>(cu2) << 6) & 0xfff) + (cu3 & 0x3f);
    }
    else if ((cu1 >> 3) == 0x1e)
    {
      const uint8_t cu2(static_cast<uint8_t>(*in_iter)); ++in_iter; check_continuation(cu2);
      const uint8_t cu3(static_cast<uint8_t>(*in_iter)); ++in_iter; check_continuation(cu3);
      const uint8_t cu4(static_cast<uint8_t>(*in_iter)); ++in_iter; check_continuation(cu4);
      return ((static_cast<uint32_t>(cu1) << 18) & 0x1fffff) + ((static_cast<uint32_t>(cu2) << 12) & 0x3ffff)
        + ((static_cast<uint32_t>(cu3) << 6) & 0xfff) + (cu4 & 0x3f);
    }
    else throw std::runtime_error("UTF-8 error");
  }

  template <typename OutputIterator>
  static void set_code_point(OutputIterator& out_iter, const uint32_t cp)
  {
    static_assert(sizeof(typename std::iterator_traits<OutputIterator>::value_type) == sizeof(uint8_t), "UTF-8 error");
    if (cp < 0x80)
    {
      *out_iter = static_cast<uint8_t>(cp); ++out_iter;
    }
    else if (cp < 0x800)
    {
      *out_iter = static_cast<uint8_t>((cp >> 6) | 0xc0); ++out_iter;
      *out_iter = static_cast<uint8_t>((cp & 0x3f) | 0x80); ++out_iter;
    }
    else if (cp < 0x10000)
    {
      *out_iter = static_cast<uint8_t>((cp >> 12) | 0xe0); ++out_iter;
      *out_iter = static_cast<uint8_t>(((cp >> 6) & 0x3f) | 0x80); ++out_iter;
      *out_iter = static_cast<uint8_t>((cp & 0x3f) | 0x80); ++out_iter;
    }
    else if (cp < 0x10ffff)
    {
      *out_iter = static_cast<uint8_t>((cp >> 18) | 0xf0); ++out_iter;
      *out_iter = static_cast<uint8_t>(((cp >> 12) & 0x3f) | 0x80); ++out_iter;
      *out_iter = static_cast<uint8_t>(((cp >> 6) & 0x3f) | 0x80); ++out_iter;
      *out_iter = static_cast<uint8_t>((cp & 0x3f) | 0x80); ++out_iter;
    }
    else throw std::runtime_error("UTF-8 error");
  }
}; // utf8

} } } // brig::unicode::detail

#endif // BRIG_UNICODE_DETAIL_UTF8_HPP
