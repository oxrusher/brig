// Andrew Naplavkov

#ifndef BRIG_STRING_CAST_HPP
#define BRIG_STRING_CAST_HPP

#include <sstream>
#include <string>

namespace brig {

template <typename CharT, typename T>
std::basic_string<CharT> string_cast(const T& v)
{
  std::basic_ostringstream<CharT> stream;
  stream << v;
  return stream.str();
}

} // brig

#endif // BRIG_STRING_CAST_HPP
