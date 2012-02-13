// Andrew Naplavkov

#ifndef BRIG_DETAIL_STRING_CAST_HPP
#define BRIG_DETAIL_STRING_CAST_HPP

#include <locale>
#include <sstream>
#include <string>

namespace brig { namespace detail {

template <typename CharT, typename T>
std::basic_string<CharT> string_cast(const T& r)
{
  std::basic_ostringstream<CharT> stream;
  stream.imbue(std::locale::classic());
  stream << r;
  return stream.str();
}

} } // brig::detail

#endif // BRIG_DETAIL_STRING_CAST_HPP
