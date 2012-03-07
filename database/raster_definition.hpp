// Andrew Naplavkov

#ifndef BRIG_DATABASE_RASTER_DEFINITION_HPP
#define BRIG_DATABASE_RASTER_DEFINITION_HPP

#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <brig/boost/geometry.hpp>
#include <brig/database/column_definition.hpp>
#include <brig/database/identifier.hpp>
#include <string>
#include <vector>

namespace brig { namespace database {

struct raster_definition {
  struct level {
    brig::boost::point resolution;
    identifier geometry_layer;
    ::boost::variant<std::string, column_definition> raster_column;
    std::string sql_filter;

    level() : resolution(0, 0)  {}
  }; // level

  identifier id;
  std::vector<level> levels;
}; // raster_definition

} } // brig::database

#endif // BRIG_DATABASE_RASTER_DEFINITION_HPP
