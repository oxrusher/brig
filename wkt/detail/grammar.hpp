// Andrew Naplavkov

#ifndef BRIG_WKT_DETAIL_GRAMMAR_HPP
#define BRIG_WKT_DETAIL_GRAMMAR_HPP

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>
#include <brig/wkt/detail/line_t.hpp>
#include <brig/wkt/detail/point_t.hpp>
#include <brig/wkt/detail/wkbgeometry.hpp>
#include <brig/wkt/detail/wkbpoint.hpp>
#include <brig/wkt/detail/wkbsequence.hpp>
#include <vector>

BOOST_FUSION_ADAPT_STRUCT(brig::wkt::detail::point_t, (double, x) (double, y))
BOOST_FUSION_ADAPT_STRUCT(brig::wkt::detail::line_t, (std::vector<brig::wkt::detail::point_t>, points))
BOOST_FUSION_ADAPT_STRUCT(brig::wkt::detail::wkbpoint, (brig::wkt::detail::point_t, point))
BOOST_FUSION_ADAPT_STRUCT(brig::wkt::detail::wkblinestring, (std::vector<brig::wkt::detail::point_t>, elements))
BOOST_FUSION_ADAPT_STRUCT(brig::wkt::detail::wkbpolygon, (std::vector<brig::wkt::detail::line_t>, elements))
BOOST_FUSION_ADAPT_STRUCT(brig::wkt::detail::wkbmultipoint, (std::vector<brig::wkt::detail::wkbpoint>, elements))
BOOST_FUSION_ADAPT_STRUCT(brig::wkt::detail::wkbmultilinestring, (std::vector<brig::wkt::detail::wkblinestring>, elements))
BOOST_FUSION_ADAPT_STRUCT(brig::wkt::detail::wkbmultipolygon, (std::vector<brig::wkt::detail::wkbpolygon>, elements))
BOOST_FUSION_ADAPT_STRUCT(brig::wkt::detail::wkbgeometry, (brig::wkt::detail::wkbvariant, var))
BOOST_FUSION_ADAPT_STRUCT(brig::wkt::detail::wkbgeometrycollection, (std::vector<brig::wkt::detail::wkbgeometry>, geoms))

namespace brig { namespace wkt { namespace detail {

using namespace boost::spirit::qi;

template <typename InputIterator>
struct grammar : boost::spirit::qi::grammar<InputIterator, wkbgeometry(), blank_type>
{
  rule<InputIterator, double(), blank_type> x, y;
  rule<InputIterator, point_t(), blank_type> point;
  rule<InputIterator, line_t(), blank_type> linestring_text;
  rule<InputIterator, wkbpoint(), blank_type> point_text, point_tagged_text;
  rule<InputIterator, wkblinestring(), blank_type> linestring_tagged_text;
  rule<InputIterator, wkbpolygon(), blank_type> polygon_text, polygon_tagged_text;
  rule<InputIterator, wkbmultipoint(), blank_type> multipoint_text, multipoint_tagged_text;
  rule<InputIterator, wkbmultilinestring(), blank_type> multilinestring_text, multilinestring_tagged_text;
  rule<InputIterator, wkbmultipolygon(), blank_type> multipolygon_text, multipolygon_tagged_text;
  rule<InputIterator, wkbgeometrycollection(), blank_type> geometrycollection_text, geometrycollection_tagged_text;
  rule<InputIterator, wkbgeometry(), blank_type> geometry_tagged_text;
  
  grammar() : grammar::base_type(geometry_tagged_text)
  {
    using boost::phoenix::at_c;
    using boost::phoenix::push_back;

    geometry_tagged_text =
      point_tagged_text |
      linestring_tagged_text |
      polygon_tagged_text |
      multipoint_tagged_text |
      multilinestring_tagged_text |
      multipolygon_tagged_text |
      geometrycollection_tagged_text;
    point_tagged_text = "POINT" >> point_text[ _val = _1 ];
    linestring_tagged_text = "LINESTRING" >> linestring_text[ at_c<0>(_val) = at_c<0>(_1) ];
    polygon_tagged_text = "POLYGON" >> polygon_text[ _val = _1 ];
    multipoint_tagged_text = "MULTIPOINT" >> multipoint_text[ _val = _1 ];
    multilinestring_tagged_text = "MULTILINESTRING" >> multilinestring_text[ _val = _1 ];
    multipolygon_tagged_text = "MULTIPOLYGON" >> multipolygon_text[ _val = _1 ];
    geometrycollection_tagged_text = "GEOMETRYCOLLECTION" >> geometrycollection_text[ _val = _1 ];
    point_text = '(' >> point >> ')' | eps;
    point = x >> y;
    x = double_;
    y = double_;
    linestring_text = '(' >> point[ push_back(at_c<0>(_val), _1) ] >> *( ',' >> point[ push_back(at_c<0>(_val), _1) ]) >> ')' | eps;
    polygon_text = '(' >> linestring_text[ push_back(at_c<0>(_val), _1) ] >> *(',' >> linestring_text[ push_back(at_c<0>(_val), _1) ]) >> ')' | eps;
    multipoint_text = '(' >> point_text[ push_back(at_c<0>(_val), _1) ] >> *(',' >> point_text[ push_back(at_c<0>(_val), _1) ]) >> ')' | eps;
    multilinestring_text = '(' >> linestring_text[ push_back(at_c<0>(_val), _1) ] >> *(',' >> linestring_text[ push_back(at_c<0>(_val), _1) ]) >> ')' | eps;
    multipolygon_text = '(' >> polygon_text[ push_back(at_c<0>(_val), _1) ] >> *(',' >> polygon_text[ push_back(at_c<0>(_val), _1) ]) >> ')' | eps;
    geometrycollection_text = '(' >> geometry_tagged_text[ push_back(at_c<0>(_val), _1) ] >> *(',' >> geometry_tagged_text[ push_back(at_c<0>(_val), _1) ]) >> ')' | eps;
  }
};

} } } // brig::wkt::detail

#endif // BRIG_WKT_DETAIL_GRAMMAR_HPP
