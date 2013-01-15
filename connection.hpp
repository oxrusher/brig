// Andrew Naplavkov

#ifndef BRIG_CONNECTION_HPP
#define BRIG_CONNECTION_HPP

#include <boost/utility.hpp>
#include <brig/boost/geometry.hpp>
#include <brig/identifier.hpp>
#include <brig/inserter.hpp>
#include <brig/raster_pyramid.hpp>
#include <brig/rowset.hpp>
#include <brig/table_definition.hpp>
#include <memory>
#include <string>
#include <vector>

namespace brig {

struct connection : ::boost::noncopyable
{
  virtual ~connection()  {}

  virtual std::vector<identifier> get_tables() = 0;
  virtual std::vector<identifier> get_geometry_layers() = 0;
  virtual std::vector<raster_pyramid> get_raster_layers() = 0;
  virtual table_definition get_table_definition(const identifier& tbl) = 0;

  virtual brig::boost::box get_mbr(const table_definition& tbl, const std::string& col) = 0;
  virtual std::shared_ptr<rowset> select(const table_definition& tbl) = 0;

  /*!
  AFTER CALL: define bounding box with boost::as_binary() if column_definition.query_value is empty blob_t
  */
  virtual table_definition fit_to_create(const table_definition& tbl) = 0;
  virtual void create(const table_definition& tbl) = 0;
  virtual void drop(const table_definition& tbl) = 0;

  virtual raster_pyramid fit_to_reg(const raster_pyramid& raster) = 0;
  virtual void reg(const raster_pyramid& raster) = 0;
  virtual void unreg(const raster_pyramid& raster) = 0;

  virtual std::shared_ptr<inserter> get_inserter(const table_definition& tbl) = 0;
}; // connection

} // brig

#endif // BRIG_CONNECTION_HPP
