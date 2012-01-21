// Andrew Naplavkov

#ifndef BRIG_WKT_SCAN_HPP
#define BRIG_WKT_SCAN_HPP

#include <boost/spirit/include/qi.hpp>
#include <brig/wkt/detail/grammar.hpp>
#include <brig/wkt/detail/wkbgeometry.hpp>
#include <stdexcept>
#include <string>

namespace brig { namespace wkt {

template <typename OutputIterator>
void scan(const std::string& in_wkt, OutputIterator& out_iter)
{
  auto in_beg = in_wkt.begin(), in_end = in_wkt.end();
  detail::wkt_grammar<decltype(in_beg)> gr;
  detail::wkbgeometry wkb;
  if (boost::spirit::qi::phrase_parse(in_beg, in_end, gr, boost::spirit::qi::blank, wkb) && in_beg == in_end)
    detail::set<>(out_iter, wkb);
  else
    throw std::runtime_error("wkt error");
}

} } // brig::wkt

#endif // BRIG_WKT_SCAN_HPP
