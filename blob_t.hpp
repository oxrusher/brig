// Andrew Naplavkov

#ifndef BRIG_BLOB_T_HPP
#define BRIG_BLOB_T_HPP

#include <algorithm>
#include <cstdint>
#include <iomanip>
#include <ios>
#include <locale>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

namespace brig {

typedef std::vector<uint8_t> blob_t;

} // brig

namespace std {

template <typename CharT, typename TraitsT>
basic_ostream<CharT, TraitsT>& operator<<(basic_ostream<CharT, TraitsT>& stream, const brig::blob_t& blob)
{
  if (stream.flags() & ios::hex)
  {
    const size_t width(size_t(stream.width()));
    const size_t size(blob.size());
    const size_t count(width == 0? size: min<>(width / 2, size));

    basic_ostringstream<CharT, TraitsT> s; s.imbue(locale::classic());
    s << hex << setfill(CharT(0x30)); // DIGIT ZERO
    if (stream.flags() & ios::uppercase) s << uppercase;
    for (size_t i(0); i < count; ++i)
      s << setw(2) << static_cast<int>(blob[i]);
    stream << s.str();
  }
  else
  {
    static const CharT prefix[] = { 0x42, 0x4c, 0x4f, 0x42, 0x20, 0x73, 0x7a, 0x20, 0x3d, 0x20, 0 }; // "BLOB sz = "
    stream << prefix << blob.size();
  }
  return stream;
}

} // std

#endif // BRIG_BLOB_T_HPP
