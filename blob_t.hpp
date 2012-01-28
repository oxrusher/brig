// Andrew Naplavkov

#ifndef BRIG_BLOB_T_HPP
#define BRIG_BLOB_T_HPP

#include <cstdint>
#include <iomanip>
#include <ios>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

namespace brig {

struct blob_t : std::vector<uint8_t>  {};

} // brig

namespace std {

template <typename CharT, typename TraitsT>
basic_ostream<CharT, TraitsT>& operator<<(basic_ostream<CharT, TraitsT>& out_stream, const brig::blob_t& in_blob)
{
  if (out_stream.flags() & ios::hex)
  {
    basic_ostringstream<CharT, TraitsT> str_stream;
    str_stream << hex << setfill(CharT(0x30));
    for (size_t i(0); i < in_blob.size(); ++i)
      str_stream << setw(2) << static_cast<uint32_t>(in_blob[i]);
    out_stream << str_stream.str();
  }
  else
  {
    static const CharT prefix[] = { 0x42, 0x4c, 0x4f, 0x42, 0x20, 0x73, 0x7a, 0x20, 0x3d, 0x20, 0 }; // "BLOB sz = "
    out_stream << prefix << in_blob.size();
  }
  return out_stream;
}

} // std

#endif // BRIG_BLOB_T_HPP
