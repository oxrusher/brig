// Andrew Naplavkov

#ifndef BRIG_DATABASE_COLUMN_DEFINITION_HPP
#define BRIG_DATABASE_COLUMN_DEFINITION_HPP

#include <algorithm>
#include <brig/database/global.hpp>
#include <brig/database/identifier.hpp>
#include <brig/database/variant.hpp>
#include <iterator>
#include <string>

namespace brig { namespace database {

struct column_definition {
  std::string name;
  column_type type;
  identifier dbms_type_lcase;
  int chars, srid, epsg;
  bool not_null;

  std::string query_expression;
  variant query_value;

  column_definition() : type(VoidColumn), chars(-1), srid(-1), epsg(-1), not_null(false)  {}
}; // column_definition

template <typename Iterator>
typename std::iterator_traits<Iterator>::pointer find_column(Iterator beg, Iterator end, const std::string& col_name)
{
  auto col_iter(std::find_if(beg, end, [&col_name](const column_definition& col){ return col_name.compare(col.name) == 0; }));
  return col_iter == end? 0: &*col_iter;
}

} } // brig::database

#endif // BRIG_DATABASE_COLUMN_DEFINITION_HPP
