// Andrew Naplavkov

#ifndef BRIG_DATABASE_RASTER_LEVEL_HPP
#define BRIG_DATABASE_RASTER_LEVEL_HPP

#include <brig/boost/geometry.hpp>
#include <brig/database/column_definition.hpp>
#include <brig/database/identifier.hpp>
#include <vector>

namespace brig { namespace database {

struct raster_level {
  brig::boost::point resolution;
  identifier geometry;
  column_definition raster;

  std::vector<column_definition> query_conditions;

  raster_level() : resolution(0, 0)  {}
}; // raster_level

} } // brig::database

#endif // BRIG_DATABASE_RASTER_LEVEL_HPP
