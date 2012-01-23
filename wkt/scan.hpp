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
#include <type_traits>
#include <vector>

namespace brig { namespace wkt {

inline void scan(const std::string& in_wkt, std::vector<uint8_t>& out_wkb)
{
  auto in_beg = in_wkt.begin(), in_end = in_wkt.end();
  brig::detail::back_insert_iterator<std::remove_reference<decltype(out_wkb)>::type> out_iter(out_wkb);
  detail::grammar<decltype(in_beg)> gr;
  detail::wkbgeometry geom;
  if (boost::spirit::qi::phrase_parse(in_beg, in_end, gr, boost::spirit::qi::blank, geom) && in_beg == in_end)
    detail::set<>(out_iter, geom);
  else
    throw std::runtime_error("wkt error");
}

} } // brig::wkt

#endif // BRIG_WKT_SCAN_HPP
