// Andrew Naplavkov

#ifndef BRIG_DETAIL_TO_STRING_HPP
#define BRIG_DETAIL_TO_STRING_HPP

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>

namespace brig { namespace detail {

inline std::string to_string(int64_t val)
{
  char buf[22];
  memset(buf, 0, sizeof(buf));
  sprintf(buf, "%d", val);
  return buf;
}

} } // brig::detail

#endif // BRIG_DETAIL_TO_STRING_HPP
