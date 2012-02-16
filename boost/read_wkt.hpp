// Andrew Naplavkov

#ifndef BRIG_BOOST_READ_WKT_HPP
#define BRIG_BOOST_READ_WKT_HPP

#include <boost/spirit/include/qi.hpp>
#include <brig/boost/detail/grammar.hpp>
#include <brig/boost/geometry.hpp>
#include <stdexcept>
#include <string>

namespace brig { namespace boost {

inline void read_wkt(const char* in_wkt, geometry& out_geom)
{
  using namespace ::boost::spirit::qi;
  detail::grammar<const char*> gr;
  if (!phrase_parse(in_wkt, (const char*)0, gr, blank, out_geom) || *in_wkt != 0)
    throw std::runtime_error("wkt error");
}

inline void read_wkt(const std::string& in_wkt, geometry& out_geom)
{
  auto in_ptr = in_wkt.c_str();
  read_wkt(in_ptr, out_geom);
}

} } // brig::boost

#endif // BRIG_BOOST_READ_WKT_HPP
