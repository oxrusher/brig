// Andrew Naplavkov

#ifndef BRIG_WKT_SCAN_HPP
#define BRIG_WKT_SCAN_HPP

#include <boost/spirit/include/qi.hpp>
#include <brig/detail/back_insert_iterator.hpp>
#include <brig/wkt/detail/grammar.hpp>
#include <brig/wkt/detail/wkbgeometry.hpp>
#include <stdexcept>
#include <string>
#include <vector>

namespace brig { namespace wkt {

template <typename T>
void scan(const char* in_wkt, std::vector<T>& out_wkb)
{
  detail::grammar<const char*> gr;
  detail::wkbgeometry geom;
  if (!boost::spirit::qi::phrase_parse(in_wkt, (const char*)0, gr, boost::spirit::qi::blank, geom) || *in_wkt != 0)
    throw std::runtime_error("wkt error");
  out_wkb.clear();
  auto out_iter = brig::detail::back_inserter(out_wkb);
  detail::set<>(out_iter, geom);
}

template <typename T>
void scan(const std::string& in_wkt, std::vector<T>& out_wkb)
{
  const char* in_ptr(in_wkt.c_str());
  scan(in_ptr, out_wkb);
}

} } // brig::wkt

#endif // BRIG_WKT_SCAN_HPP
