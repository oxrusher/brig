// Andrew Naplavkov

#ifndef BRIG_UNICODE_DETAIL_UTF16_HPP
#define BRIG_UNICODE_DETAIL_UTF16_HPP

#include <cstdint>
#include <iterator>
#include <stdexcept>

namespace brig { namespace unicode { namespace detail {

class utf16 {
public:
  template <typename InputIterator>
  static uint32_t read_code_point(InputIterator& itr)
  {
    static_assert(sizeof(typename std::iterator_traits<InputIterator>::value_type) == sizeof(uint16_t), "UTF-16 error");
    const uint16_t cu1 = read_code_unit(itr);
    if (is_high(cu1))
    {
      const uint16_t cu2 = read_code_unit(itr);
      if (is_low(cu2))
        return (static_cast<uint32_t>(cu1) << 10) + cu2 + (0x10000 - (0xd800 << 10) - 0xdc00);
    }
    else if (!is_low(cu1))
      return static_cast<uint32_t>(cu1);
    throw std::runtime_error("UTF-16 error");
  }

  template <typename OutputIterator>
  static void write_code_point(OutputIterator& itr, const uint32_t cp)
  {
    static_assert(sizeof(typename std::iterator_traits<OutputIterator>::value_type) == sizeof(uint16_t), "UTF-16 error");
    if (cp < 0xffff)
    {
      if (is_low(cp)) throw std::runtime_error("UTF-16 error");
      write_code_unit(itr, cp);
    }
    else if (cp < 0x10ffff)
    {
      write_code_unit(itr, 0xd800 - (0x10000 >> 10) + (cp >> 10));
      write_code_unit(itr, 0xdc00 + (cp & 0x3ff));
    }
    else
      throw std::runtime_error("UTF-16 error");
  }

private:
  static bool is_high(const uint16_t cu) { return 0xd800 <= cu && cu <= 0xdbff; }
  static bool is_low(const uint16_t cu) { return 0xdc00 <= cu && cu <= 0xdfff; }

  template <typename InputIterator>
  static uint16_t read_code_unit(InputIterator& itr)
  {
    uint16_t cu = static_cast<uint16_t>(*itr);
    ++itr;
    return cu;
  }

  template <typename OutputIterator, typename CodeUnit>
  static void write_code_unit(OutputIterator& itr, const CodeUnit cu)
  {
    *itr = static_cast<uint16_t>(cu);
    ++itr;
  }
}; // utf16

} } } // brig::unicode::detail

#endif // BRIG_UNICODE_DETAIL_UTF16_HPP
