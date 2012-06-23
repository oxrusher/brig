// Andrew Naplavkov

#ifndef BRIG_DATABASE_RASTER_LEVLEL_HPP
#define BRIG_DATABASE_RASTER_LEVLEL_HPP

#include <brig/boost/geometry.hpp>
#include <brig/database/alias.hpp>
#include <brig/database/identifier.hpp>
#include <brig/database/variant.hpp>
#include <string>
#include <vector>
#include <utility>

namespace brig { namespace database {

struct raster_level
{
  brig::boost::point resolution;
  identifier geometry;
  alias raster;

  std::vector<std::pair<alias, variant>> query_conditions;

  raster_level() : resolution(0, 0)  {}
}; // raster_level

} } // brig::database

#endif // BRIG_DATABASE_RASTER_LEVLEL_HPP
