// Andrew Naplavkov

#ifndef BRIG_UNICODE_DETAIL_UTF8_HPP
#define BRIG_UNICODE_DETAIL_UTF8_HPP

#include <cstdint>
#include <iterator>
#include <stdexcept>

namespace brig { namespace unicode { namespace detail {

class utf8 {
public:
  template <typename InputIterator>
  static uint32_t read_code_point(InputIterator& itr)
  {
    static_assert(sizeof(typename std::iterator_traits<InputIterator>::value_type) == sizeof(uint8_t), "UTF-8 error");
    const uint8_t cu1 = read_code_unit(itr);
    if (cu1 < 0x80)
      return static_cast<uint32_t>(cu1);
    else if ((cu1 >> 5) == 0x6)
    {
      const uint8_t cu2 = read_continuation(itr);
      return ((static_cast<uint32_t>(cu1) << 6) & 0x7ff) + (cu2 & 0x3f);
    }
    else if ((cu1 >> 4) == 0xe)
    {
      const uint8_t cu2 = read_continuation(itr);
      const uint8_t cu3 = read_continuation(itr);
      return ((static_cast<uint32_t>(cu1) << 12) & 0xffff)
        + ((static_cast<uint32_t>(cu2) << 6) & 0xfff)
        + (cu3 & 0x3f);
    }
    else if ((cu1 >> 3) == 0x1e)
    {
      const uint8_t cu2 = read_continuation(itr);
      const uint8_t cu3 = read_continuation(itr);
      const uint8_t cu4 = read_continuation(itr);
      return ((static_cast<uint32_t>(cu1) << 18) & 0x1fffff)
        + ((static_cast<uint32_t>(cu2) << 12) & 0x3ffff)
        + ((static_cast<uint32_t>(cu3) << 6) & 0xfff)
        + (cu4 & 0x3f);
    }
    else
      throw std::runtime_error("UTF-8 error");
  }

  template <typename OutputIterator>
  static void write_code_point(OutputIterator& itr, const uint32_t cp)
  {
    static_assert(sizeof(typename std::iterator_traits<OutputIterator>::value_type) == sizeof(uint8_t), "UTF-8 error");
    if (cp < 0x80)
      write_code_unit(itr, cp);
    else if (cp < 0x800)
    {
      write_code_unit(itr, (cp >> 6) | 0xc0);
      write_code_unit(itr, (cp & 0x3f) | 0x80);
    }
    else if (cp < 0x10000)
    {
      write_code_unit(itr, (cp >> 12) | 0xe0);
      write_code_unit(itr, ((cp >> 6) & 0x3f) | 0x80);
      write_code_unit(itr, (cp & 0x3f) | 0x80);
    }
    else if (cp < 0x10ffff)
    {
      write_code_unit(itr, (cp >> 18) | 0xf0);
      write_code_unit(itr, ((cp >> 12) & 0x3f) | 0x80);
      write_code_unit(itr, ((cp >> 6) & 0x3f) | 0x80);
      write_code_unit(itr, (cp & 0x3f) | 0x80);
    }
    else
      throw std::runtime_error("UTF-8 error");
  }

private:
  template <typename InputIterator>
  static uint8_t read_code_unit(InputIterator& itr)
  {
    uint8_t cu = static_cast<uint8_t>(*itr);
    ++itr;
    return cu;
  }

  template <typename InputIterator>
  static uint8_t read_continuation(InputIterator& itr)
  {
    uint8_t cu = read_code_unit(itr);
    if ((cu >> 6) != 0x2) throw std::runtime_error("UTF-8 error");
    return cu;
  }

  template <typename OutputIterator, typename CodeUnit>
  static void write_code_unit(OutputIterator& itr, const CodeUnit cu)
  {
    *itr = static_cast<uint8_t>(cu);
    ++itr;
  }
}; // utf8

} } } // brig::unicode::detail

#endif // BRIG_UNICODE_DETAIL_UTF8_HPP
