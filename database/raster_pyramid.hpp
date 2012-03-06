// Andrew Naplavkov

#ifndef BRIG_DATABASE_RASTER_PYRAMID_HPP
#define BRIG_DATABASE_RASTER_PYRAMID_HPP

#include <brig/database/identifier.hpp>
#include <vector>

namespace brig { namespace database {

template <typename Raster>
struct raster_pyramid : std::vector<Raster>  { identifier base; };

} } // brig::database

#endif // BRIG_DATABASE_RASTER_PYRAMID_HPP
