// Andrew Naplavkov

#ifndef BRIG_DATABASE_RASTER_DETAIL_HPP
#define BRIG_DATABASE_RASTER_DETAIL_HPP

#include <boost/variant/get.hpp>
#include <boost/variant/variant.hpp>
#include <brig/boost/geometry.hpp>
#include <brig/database/column_detail.hpp>
#include <brig/database/identifier.hpp>
#include <string>

namespace brig { namespace database {

struct raster_detail {
  brig::boost::point resolution;
  identifier geometry_layer;
  ::boost::variant<std::string, column_detail> raster_column;
  std::string sql_filter;

  raster_detail() : resolution(0, 0)  {}
}; // raster_detail

} } // brig::database

#endif // BRIG_DATABASE_RASTER_DETAIL_HPP
