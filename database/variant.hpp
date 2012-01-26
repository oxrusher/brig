// Andrew Naplavkov

#ifndef BRIG_DATABASE_VARIANT_HPP
#define BRIG_DATABASE_VARIANT_HPP

#include "boost/date_time/gregorian/gregorian.hpp"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <brig/blob_t.hpp>
#include <cstdint>
#include <ostream>
#include <string>

namespace brig { namespace database {

struct null_t  { null_t() {} };

typedef boost::variant<
  null_t,
  int16_t,
  int32_t,
  int64_t,
  float,
  double,
  std::string, // UTF-8
  blob_t,
  boost::gregorian::date,
  boost::posix_time::ptime
> variant;

} } // brig::database

namespace std {

template <class CharT, class TraitsT>
basic_ostream<CharT, TraitsT>& operator<<(basic_ostream<CharT, TraitsT>& out_stream, const brig::database::null_t&)
{
  return out_stream;
}

} // std

#endif // BRIG_DATABASE_VARIANT_HPP
