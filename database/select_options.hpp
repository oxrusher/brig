// Andrew Naplavkov

#ifndef BRIG_DATABASE_SELECT_OPTIONS_HPP
#define BRIG_DATABASE_SELECT_OPTIONS_HPP

#include <brig/boost/geometry.hpp>
#include <brig/database/column_definition.hpp>
#include <brig/database/variant.hpp>
#include <string>
#include <vector>

namespace brig { namespace database {

struct select_options {
  std::vector<column_definition> expression_columns;
  std::string sql_filter, box_filter_column;
  brig::boost::box box_filter;
  std::vector<std::string> select_columns;
  int rows;

  select_options() : box_filter(brig::boost::point(0, 0), brig::boost::point(0, 0)), rows(-1)  {}
}; // select_options

} } // brig::database

#endif // BRIG_DATABASE_SELECT_OPTIONS_HPP
