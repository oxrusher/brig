// Andrew Naplavkov

#ifndef BRIG_DATABASE_RASTER_LEVLEL_HPP
#define BRIG_DATABASE_RASTER_LEVLEL_HPP

#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <brig/boost/geometry.hpp>
#include <brig/database/column_definition.hpp>
#include <brig/database/identifier.hpp>
#include <brig/database/variant.hpp>
#include <string>
#include <vector>

namespace brig { namespace database {

struct raster_level {
  brig::boost::point resolution;
  identifier geometry_layer;
  ::boost::variant<std::string, column_definition> raster_column;

  std::string sql_filter;
  std::vector<variant> parameters;

  raster_level() : resolution(0, 0)  {}
}; // raster_level

} } // brig::database

#endif // BRIG_DATABASE_RASTER_LEVLEL_HPP
