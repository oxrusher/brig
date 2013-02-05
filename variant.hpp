// Andrew Naplavkov

#ifndef BRIG_VARIANT_HPP
#define BRIG_VARIANT_HPP

#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <brig/blob_t.hpp>
#include <brig/null_t.hpp>
#include <cstdint>
#include <string>

namespace brig {

typedef ::boost::variant<
  null_t,
  int16_t,
  int32_t,
  int64_t,
  float,
  double,
  std::string,
  blob_t
> variant;

} // brig

#endif // BRIG_VARIANT_HPP
