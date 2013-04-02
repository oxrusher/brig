// Andrew Naplavkov

#ifndef BRIG_PROVIDER_HPP
#define BRIG_PROVIDER_HPP

#include <boost/utility.hpp>
#include <brig/boost/geometry.hpp>
#include <brig/identifier.hpp>
#include <brig/inserter.hpp>
#include <brig/pyramid_def.hpp>
#include <brig/rowset.hpp>
#include <brig/table_def.hpp>
#include <memory>
#include <string>
#include <vector>

namespace brig {

struct provider : ::boost::noncopyable
{
  virtual ~provider()  {}

  virtual std::vector<identifier> get_tables() = 0;
  virtual std::vector<identifier> get_geometry_layers() = 0;
  virtual std::vector<pyramid_def> get_raster_layers() = 0;
  virtual table_def get_table_def(const identifier& tbl) = 0;
  virtual brig::boost::box get_extent(const table_def& tbl) = 0;
  virtual std::shared_ptr<rowset> select(const table_def& tbl) = 0;

  /*!
  AFTER CALL: define bounding box with boost::as_binary() if column_def.query_value is empty blob_t
  */
  virtual table_def fit_to_create(const table_def& tbl) = 0;
  virtual void create(const table_def& tbl) = 0;
  virtual void drop(const table_def& tbl) = 0;

  virtual pyramid_def fit_to_reg(const pyramid_def& raster) = 0;
  virtual void reg(const pyramid_def& raster) = 0;
  virtual void unreg(const pyramid_def& raster) = 0;

  virtual std::shared_ptr<inserter> get_inserter(const table_def& tbl) = 0;
}; // provider

} // brig

#endif // BRIG_PROVIDER_HPP
