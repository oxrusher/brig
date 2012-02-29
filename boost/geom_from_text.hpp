// Andrew Naplavkov

#ifndef BRIG_BOOST_GEOM_FROM_TEXT_HPP
#define BRIG_BOOST_GEOM_FROM_TEXT_HPP

#include <boost/spirit/include/qi.hpp>
#include <brig/boost/detail/grammar.hpp>
#include <brig/boost/geometry.hpp>
#include <stdexcept>
#include <string>

namespace brig { namespace boost {

inline geometry geom_from_text(const char* wkt)
{
  using namespace ::boost::spirit::qi;
  detail::grammar<const char*> gr;
  geometry geom;
  if (!phrase_parse(wkt, (const char*)0, gr, blank, geom) || *wkt != 0)
    throw std::runtime_error("wkt error");
  return geom;
}

inline geometry geom_from_text(const std::string& wkt)
{
  return geom_from_text(wkt.c_str());
}

} } // brig::boost

#endif // BRIG_BOOST_GEOM_FROM_TEXT_HPP
