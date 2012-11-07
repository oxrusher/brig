// Andrew Naplavkov

#ifndef BRIG_DATABASE_VARIANT_HPP
#define BRIG_DATABASE_VARIANT_HPP

#ifdef _MSC_VER
#  define BOOST_NO_ARGUMENT_DEPENDENT_LOOKUP
#endif

#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <brig/blob_t.hpp>
#include <brig/database/null_t.hpp>
#include <cstdint>
#include <string>

namespace brig { namespace database {

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

} } // brig::database

#endif // BRIG_DATABASE_VARIANT_HPP
