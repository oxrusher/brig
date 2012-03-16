// Andrew Naplavkov

#ifndef BRIG_STRING_CAST_HPP
#define BRIG_STRING_CAST_HPP

#include <locale>
#include <sstream>
#include <string>

namespace brig {

template <typename CharT, typename T>
std::basic_string<CharT> string_cast(const T& r)
{
  std::basic_ostringstream<CharT> stream; stream.imbue(std::locale::classic());
  stream << r;
  return stream.str();
}

} // brig

#endif // BRIG_STRING_CAST_HPP
