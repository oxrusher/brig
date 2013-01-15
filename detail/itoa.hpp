// Andrew Naplavkov

// http://stackoverflow.com/questions/6713420/c-convert-integer-to-string-at-compile-time

#ifndef BRIG_DETAIL_ITOA_HPP
#define BRIG_DETAIL_ITOA_HPP

#include <boost/mpl/string.hpp>
#include <string>

namespace brig { namespace detail {

template <bool Top, size_t N>
struct itoa_impl
{
  typedef typename ::boost::mpl::push_back<typename itoa_impl<false, N / 10>::type, ::boost::mpl::char_<'0' + N % 10>>::type type;
};

template <>
struct itoa_impl<false, 0>
{
  typedef ::boost::mpl::string<> type;
};

template <>
struct itoa_impl<true, 0>
{
  typedef ::boost::mpl::string<'0'> type;
};

template <size_t N>
struct itoa
{
  typedef typename ::boost::mpl::c_str<typename itoa_impl<true, N>::type>::type type;
};

} } // brig::detail

#endif // BRIG_DETAIL_ITOA_HPP
