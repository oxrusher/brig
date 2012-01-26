// Andrew Naplavkov

#ifndef BRIG_BLOB_T_HPP
#define BRIG_BLOB_T_HPP

#include <cstdint>
#include <iomanip>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>

namespace brig {

struct blob_t : std::vector<uint8_t>  {};

} // brig

namespace std {

template <typename CharT, typename TraitsT>
basic_ostream<CharT, TraitsT>& operator<<(basic_ostream<CharT, TraitsT>& out_stream, const brig::blob_t& blob)
{
  basic_ostringstream<CharT, TraitsT> str_stream;
  str_stream << hex << setfill(CharT(0x30));
  for (size_t i(0); i < blob.size(); ++i)
    str_stream << setw(2) << static_cast<uint32_t>(blob[i]);
  out_stream << str_stream.str();
  return out_stream;
}

} // std

#endif // BRIG_BLOB_T_HPP
