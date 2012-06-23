// Andrew Naplavkov

#ifndef BRIG_DATABASE_RASTER_PYRAMID_HPP
#define BRIG_DATABASE_RASTER_PYRAMID_HPP

#include <brig/database/identifier.hpp>
#include <brig/database/raster_level.hpp>
#include <vector>

namespace brig { namespace database {

struct raster_pyramid : identifier  { std::vector<raster_level> levels; };

} } // brig::database

#endif // BRIG_DATABASE_RASTER_PYRAMID_HPP
