// Andrew Naplavkov

#ifndef BRIG_COLUMN_DEF_HPP
#define BRIG_COLUMN_DEF_HPP

#include <algorithm>
#include <brig/column_type.hpp>
#include <brig/identifier.hpp>
#include <brig/variant.hpp>
#include <iterator>
#include <string>

namespace brig {

struct column_def {
  std::string name;
  column_type type;
  identifier type_lcase;
  int chars, srid, epsg;
  std::string proj;
  bool not_null;

  std::string query_expression;
  variant query_value;

  column_def() : type(column_type::Void), chars(-1), srid(-1), epsg(-1), not_null(false)  {}
  bool is_extent_requested() const  { return column_type::Geometry == type && typeid(blob_t) == query_value.type() && ::boost::get<blob_t>(query_value).empty(); }
}; // column_def

template <typename Iterator>
typename std::iterator_traits<Iterator>::pointer find_column(Iterator beg, Iterator end, const std::string& col_name)
{
  auto col_itr(std::find_if(beg, end, [&col_name](const column_def& col){ return col_name.compare(col.name) == 0; }));
  return col_itr == end? 0: &*col_itr;
}

} // brig

#endif // BRIG_COLUMN_DEF_HPP
