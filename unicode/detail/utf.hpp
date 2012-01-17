// Andrew Naplavkov

#ifndef BRIG_UNICODE_DETAIL_UTF_HPP
#define BRIG_UNICODE_DETAIL_UTF_HPP

#include <brig/unicode/detail/utf8.hpp>
#include <brig/unicode/detail/utf16.hpp>
#include <brig/unicode/detail/utf32.hpp>
#include <cstdint>
#include <iterator>
#include <type_traits>

namespace brig { namespace unicode { namespace detail {

template <typename Iterator>
struct utf {
  typedef typename std::iterator_traits<Iterator>::value_type code_unit;
  typedef typename std::conditional<sizeof(code_unit) == sizeof(uint8_t), utf8
        , typename std::conditional<sizeof(code_unit) == sizeof(uint16_t), utf16
        , typename std::conditional<sizeof(code_unit) == sizeof(uint32_t), utf32
        , void>::type>::type>::type type;
}; // utf

} } } // brig::unicode::detail

#endif // BRIG_UNICODE_DETAIL_UTF_HPP
