// Andrew Naplavkov

#ifndef BRIG_WKT_SCAN_HPP
#define BRIG_WKT_SCAN_HPP

#include <boost/spirit/include/qi.hpp>
#include <brig/detail/back_insert_iterator.hpp>
#include <brig/wkt/detail/grammar.hpp>
#include <brig/wkt/detail/wkbgeometry.hpp>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace brig { namespace wkt {

inline void scan(const std::string& in_wkt, std::vector<uint8_t>& out_wkb)
{
  auto in_iter = in_wkt.cbegin();
  detail::grammar<decltype(in_iter)> gr;
  detail::wkbgeometry geom;
  if (!boost::spirit::qi::phrase_parse(in_iter, in_wkt.cend(), gr, boost::spirit::qi::blank, geom) || in_iter != in_wkt.cend())
    throw std::runtime_error("wkt error");

  out_wkb.clear();
  auto out_iter = brig::detail::back_inserter(out_wkb);
  detail::set<>(out_iter, geom);
}

} } // brig::wkt

#endif // BRIG_WKT_SCAN_HPP
