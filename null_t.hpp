// Andrew Naplavkov

#ifndef BRIG_NULL_T_HPP
#define BRIG_NULL_T_HPP

#include <boost/none_t.hpp>
#include <ostream>

namespace brig {

typedef ::boost::none_t null_t;

} // brig

namespace std {

template <class CharT, class TraitsT>
basic_ostream<CharT, TraitsT>& operator<<(basic_ostream<CharT, TraitsT>& stream, const brig::null_t&)
{
  return stream;
}

} // std

#endif // BRIG_NULL_T_HPP
