// Andrew Naplavkov

#ifndef BRIG_DATABASE_NULL_T_HPP
#define BRIG_DATABASE_NULL_T_HPP

#include <ostream>

namespace brig { namespace database {

struct null_t  { null_t() {} };

} } // brig::database

namespace std {

template <class CharT, class TraitsT>
basic_ostream<CharT, TraitsT>& operator<<(basic_ostream<CharT, TraitsT>& stream, const brig::database::null_t&)
{
  return stream;
}

} // std

#endif // BRIG_DATABASE_NULL_T_HPP
