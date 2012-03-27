// Andrew Naplavkov

#ifndef BRIG_BOOST_DETAIL_GRAMMAR_HPP
#define BRIG_BOOST_DETAIL_GRAMMAR_HPP

#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi.hpp>
#include <brig/boost/geometry.hpp>

namespace brig { namespace boost { namespace detail {

inline linestring to_line(const linearring& r)  { linestring l; l.assign(std::begin(r), std::end(r)); return l; }
inline void assign_outer(polygon& p, const polygon::ring_type& r)  { p.outer().assign(std::begin(r), std::end(r)); }
inline void add_inner(polygon& p, const polygon::ring_type& r)  { p.inners().push_back(r); }

template <typename InputIterator>
struct grammar : ::boost::spirit::qi::grammar<InputIterator, geometry(), ::boost::spirit::qi::blank_type>
{
  ::boost::spirit::qi::rule<InputIterator, double(), ::boost::spirit::qi::blank_type>  x, y;
  ::boost::spirit::qi::rule<InputIterator, point(), ::boost::spirit::qi::blank_type>  point_r, point_text, point_tagged_text;
  ::boost::spirit::qi::rule<InputIterator, linearring(), ::boost::spirit::qi::blank_type>  linestring_text;
  ::boost::spirit::qi::rule<InputIterator, linestring(), ::boost::spirit::qi::blank_type>  linestring_tagged_text;
  ::boost::spirit::qi::rule<InputIterator, polygon(), ::boost::spirit::qi::blank_type>  polygon_text, polygon_tagged_text;
  ::boost::spirit::qi::rule<InputIterator, multi_point(), ::boost::spirit::qi::blank_type>  multi_point_text, multi_point_tagged_text;
  ::boost::spirit::qi::rule<InputIterator, multi_linestring(), ::boost::spirit::qi::blank_type>  multi_linestring_text, multi_linestring_tagged_text;
  ::boost::spirit::qi::rule<InputIterator, multi_polygon(), ::boost::spirit::qi::blank_type>  multi_polygon_text, multi_polygon_tagged_text;
  ::boost::spirit::qi::rule<InputIterator, geometry_collection(), ::boost::spirit::qi::blank_type>  geometry_collection_text, geometry_collection_tagged_text;
  ::boost::spirit::qi::rule<InputIterator, geometry(), ::boost::spirit::qi::blank_type>  geometry_tagged_text;
  
  grammar() : grammar::base_type(geometry_tagged_text)
  {
    using namespace ::boost::spirit::qi;
    using namespace ::boost::phoenix;

    geometry_tagged_text =
      point_tagged_text |
      linestring_tagged_text |
      polygon_tagged_text |
      multi_point_tagged_text |
      multi_linestring_tagged_text |
      multi_polygon_tagged_text |
      geometry_collection_tagged_text;
    point_tagged_text = "POINT" >> point_text[ _val = _1 ];
    linestring_tagged_text = "LINESTRING" >> linestring_text[ _val = ::boost::phoenix::bind(&to_line, _1) ];
    polygon_tagged_text = "POLYGON" >> polygon_text[ _val = _1 ];
    multi_point_tagged_text = "MULTIPOINT" >> multi_point_text[ _val = _1 ];
    multi_linestring_tagged_text = "MULTILINESTRING" >> multi_linestring_text[ _val = _1 ];
    multi_polygon_tagged_text = "MULTIPOLYGON" >> multi_polygon_text[ _val = _1 ];
    geometry_collection_tagged_text = "GEOMETRYCOLLECTION" >> geometry_collection_text[ _val = _1 ];
    point_text = '(' >> point_r >> ')' | eps;
    point_r = x[ ::boost::phoenix::bind(&point::set<0>, _val, _1) ] >> y[ ::boost::phoenix::bind(&point::set<1>, _val, _1) ];
    x = double_;
    y = double_;
    linestring_text = '(' >> point_r[ push_back(_val, _1) ] >> *( ',' >> point_r[ push_back(_val, _1) ]) >> ')' | eps;
    polygon_text = '(' >> linestring_text[ ::boost::phoenix::bind(&assign_outer, _val, _1) ] >> *(',' >> linestring_text[ ::boost::phoenix::bind(&add_inner, _val, _1) ]) >> ')' | eps;
    multi_point_text = '(' >> point_text[ push_back(_val, _1) ] >> *(',' >> point_text[ push_back(_val, _1) ]) >> ')' | eps;
    multi_linestring_text = '(' >> linestring_text[ push_back(_val, ::boost::phoenix::bind(&to_line, _1)) ] >> *(',' >> linestring_text[ push_back(_val, ::boost::phoenix::bind(&to_line, _1)) ]) >> ')' | eps;
    multi_polygon_text = '(' >> polygon_text[ push_back(_val, _1) ] >> *(',' >> polygon_text[ push_back(_val, _1) ]) >> ')' | eps;
    geometry_collection_text = '(' >> geometry_tagged_text[ push_back(_val, _1) ] >> *(',' >> geometry_tagged_text[ push_back(_val, _1) ]) >> ')' | eps;
  }
};

} } } // brig::boost::detail

#endif // BRIG_BOOST_DETAIL_GRAMMAR_HPP
