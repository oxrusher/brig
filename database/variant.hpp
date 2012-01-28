// Andrew Naplavkov

#ifndef BRIG_DATABASE_VARIANT_HPP
#define BRIG_DATABASE_VARIANT_HPP

#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <brig/blob_t.hpp>
#include <brig/database/null_t.hpp>
#include <cstdint>
#include <string>

namespace brig { namespace database {

typedef boost::variant<
  null_t,
  int16_t,
  int32_t,
  int64_t,
  float,
  double,
  boost::gregorian::date,
  boost::posix_time::ptime,
  std::string, // UTF-8
  blob_t
> variant;

} } // brig::database

#endif // BRIG_DATABASE_VARIANT_HPP
