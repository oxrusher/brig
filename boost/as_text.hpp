// Andrew Naplavkov

#ifndef BRIG_BOOST_AS_TEXT_HPP
#define BRIG_BOOST_AS_TEXT_HPP

#include <brig/boost/as_binary.hpp>
#include <brig/boost/print.hpp>
#include <string>

namespace brig { namespace boost {

template <typename Geometry>
std::string as_text(const Geometry& g)
{
  return print(as_binary(g));
}

} } // brig::boost

#endif // BRIG_BOOST_AS_TEXT_HPP
