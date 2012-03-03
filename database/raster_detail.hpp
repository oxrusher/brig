// Andrew Naplavkov

#ifndef BRIG_DATABASE_RASTER_DETAIL_HPP
#define BRIG_DATABASE_RASTER_DETAIL_HPP

#include <brig/boost/geometry.hpp>
#include <brig/database/column_detail.hpp>
#include <brig/database/object.hpp>
#include <brig/database/variant.hpp>
#include <string>
#include <vector>

namespace brig { namespace database {

struct raster_detail {
  brig::boost::point resolution;
  subobject geometry_layer;
  std::string sql_filter;
  std::vector<variant> parameters;
  column_detail raster_column;

  raster_detail() : resolution(0, 0)  {}
}; // raster_detail

struct raster_pyramid : std::vector<raster_detail>  { subobject base; };

} } // brig::database

#endif // BRIG_DATABASE_RASTER_DETAIL_HPP
