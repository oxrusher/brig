// Andrew Naplavkov

#ifndef BRIG_BOOST_DETAIL_GRAMMAR_HPP
#define BRIG_BOOST_DETAIL_GRAMMAR_HPP

#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>
#include <brig/boost/geometry.hpp>

namespace brig { namespace boost { namespace detail {

inline linestring to_line(const linearring& r)  { linestring l; l.assign(r.begin(), r.end()); return l; }
inline void assign_outer(polygon& p, const polygon::ring_type& r)  { p.outer().assign(r.begin(), r.end()); }
inline void add_inner(polygon& p, const polygon::ring_type& r)  { p.inners().push_back(r); }

template <typename InputIterator>
struct grammar : ::boost::spirit::qi::grammar<InputIterator, geometry(), ::boost::spirit::qi::blank_type>
{
  ::boost::spirit::qi::rule<InputIterator, double(), ::boost::spirit::qi::blank_type>  x, y;
  ::boost::spirit::qi::rule<InputIterator, point(), ::boost::spirit::qi::blank_type>  point_r, point_text, point_tagged_text;
  ::boost::spirit::qi::rule<InputIterator, linearring(), ::boost::spirit::qi::blank_type>  linestring_text;
  ::boost::spirit::qi::rule<InputIterator, linestring(), ::boost::spirit::qi::blank_type>  linestring_tagged_text;
  ::boost::spirit::qi::rule<InputIterator, polygon(), ::boost::spirit::qi::blank_type>  polygon_text, polygon_tagged_text;
  ::boost::spirit::qi::rule<InputIterator, multipoint(), ::boost::spirit::qi::blank_type>  multipoint_text, multipoint_tagged_text;
  ::boost::spirit::qi::rule<InputIterator, multilinestring(), ::boost::spirit::qi::blank_type>  multilinestring_text, multilinestring_tagged_text;
  ::boost::spirit::qi::rule<InputIterator, multipolygon(), ::boost::spirit::qi::blank_type>  multipolygon_text, multipolygon_tagged_text;
  ::boost::spirit::qi::rule<InputIterator, geometrycollection(), ::boost::spirit::qi::blank_type>  geometrycollection_text, geometrycollection_tagged_text;
  ::boost::spirit::qi::rule<InputIterator, geometry(), ::boost::spirit::qi::blank_type>  geometry_tagged_text;
  
  grammar() : grammar::base_type(geometry_tagged_text)
  {
    using namespace ::boost::spirit::qi;
    using namespace ::boost::phoenix;

    geometry_tagged_text =
      point_tagged_text |
      linestring_tagged_text |
      polygon_tagged_text |
      multipoint_tagged_text |
      multilinestring_tagged_text |
      multipolygon_tagged_text |
      geometrycollection_tagged_text;
    point_tagged_text = "POINT" >> point_text[ _val = _1 ];
    linestring_tagged_text = "LINESTRING" >> linestring_text[ _val = ::boost::phoenix::bind(&to_line, _1) ];
    polygon_tagged_text = "POLYGON" >> polygon_text[ _val = _1 ];
    multipoint_tagged_text = "MULTIPOINT" >> multipoint_text[ _val = _1 ];
    multilinestring_tagged_text = "MULTILINESTRING" >> multilinestring_text[ _val = _1 ];
    multipolygon_tagged_text = "MULTIPOLYGON" >> multipolygon_text[ _val = _1 ];
    geometrycollection_tagged_text = "GEOMETRYCOLLECTION" >> geometrycollection_text[ _val = _1 ];
    point_text = '(' >> point_r >> ')' | eps;
    point_r = x[ ::boost::phoenix::bind(&point::set<0>, _val, _1) ] >> y[ ::boost::phoenix::bind(&point::set<1>, _val, _1) ];
    x = double_;
    y = double_;
    linestring_text = '(' >> point_r[ push_back(_val, _1) ] >> *( ',' >> point_r[ push_back(_val, _1) ]) >> ')' | eps;
    polygon_text = '(' >> linestring_text[ ::boost::phoenix::bind(&assign_outer, _val, _1) ] >> *(',' >> linestring_text[ ::boost::phoenix::bind(&add_inner, _val, _1) ]) >> ')' | eps;
    multipoint_text = '(' >> point_text[ push_back(_val, _1) ] >> *(',' >> point_text[ push_back(_val, _1) ]) >> ')' | eps;
    multilinestring_text = '(' >> linestring_text[ push_back(_val, ::boost::phoenix::bind(&to_line, _1)) ] >> *(',' >> linestring_text[ push_back(_val, ::boost::phoenix::bind(&to_line, _1)) ]) >> ')' | eps;
    multipolygon_text = '(' >> polygon_text[ push_back(_val, _1) ] >> *(',' >> polygon_text[ push_back(_val, _1) ]) >> ')' | eps;
    geometrycollection_text = '(' >> geometry_tagged_text[ push_back(_val, _1) ] >> *(',' >> geometry_tagged_text[ push_back(_val, _1) ]) >> ')' | eps;
  }
};

} } } // brig::boost::detail

#endif // BRIG_BOOST_DETAIL_GRAMMAR_HPP
