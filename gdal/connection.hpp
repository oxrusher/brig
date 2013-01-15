// Andrew Naplavkov

#ifndef BRIG_GDAL_CONNECTION_HPP
#define BRIG_GDAL_CONNECTION_HPP

#include <brig/connection.hpp>
#include <brig/gdal/detail/datasource_allocator.hpp>
#include <brig/gdal/detail/lib.hpp>
#include <brig/gdal/detail/inserter.hpp>
#include <brig/gdal/detail/threaded_rowset.hpp>
#include <brig/unicode/lower_case.hpp>
#include <brig/unicode/transform.hpp>
#include <iterator>
#include <memory>
#include <stdexcept>

namespace brig { namespace gdal {

template <bool Threading>
class connection : public brig::connection {
  std::shared_ptr<detail::datasource_allocator> m_allocator;
  std::string m_fit_identifier;

public:
  connection(const std::string& ds, const std::string& drv = std::string(), const std::string& fit_identifier = std::string())
    : m_allocator(std::make_shared<detail::datasource_allocator>(ds, drv)), m_fit_identifier(fit_identifier)
    {}

  std::vector<identifier> get_tables() override;
  std::vector<identifier> get_geometry_layers() override;
  std::vector<raster_pyramid> get_raster_layers() override  { return std::vector<raster_pyramid>(); }
  table_definition get_table_definition(const identifier& tbl) override;

  brig::boost::box get_mbr(const table_definition& tbl, const std::string& col) override;
  std::shared_ptr<rowset> select(const table_definition& tbl) override;

  table_definition fit_to_create(const table_definition& tbl) override;
  void create(const table_definition& tbl) override;
  void drop(const table_definition& tbl) override;
  
  raster_pyramid fit_to_reg(const raster_pyramid&) override  { throw std::runtime_error("GDAL error"); }
  void reg(const raster_pyramid&) override  { throw std::runtime_error("GDAL error"); }
  void unreg(const raster_pyramid&) override  { throw std::runtime_error("GDAL error"); }

  std::shared_ptr<inserter> get_inserter(const table_definition& tbl) override;
}; // connection

template <bool Threading>
std::vector<identifier> connection<Threading>::get_tables()
{
  std::unique_ptr<detail::datasource> ds(m_allocator->allocate(false));
  return ds->get_tables();
}

template <bool Threading>
std::vector<identifier> connection<Threading>::get_geometry_layers()
{
  std::unique_ptr<detail::datasource> ds(m_allocator->allocate(false));
  return ds->get_geometry_layers();
}

template <bool Threading>
table_definition connection<Threading>::get_table_definition(const identifier& tbl)
{
  std::unique_ptr<detail::datasource> ds(m_allocator->allocate(false));
  return ds->get_table_definition(tbl);
}

template <bool Threading>
brig::boost::box connection<Threading>::get_mbr(const table_definition& tbl, const std::string& col)
{
  std::unique_ptr<detail::datasource> ds(m_allocator->allocate(false));
  return ds->get_mbr(tbl, col);
}

template <>
inline std::shared_ptr<rowset> connection<false>::select(const table_definition& tbl)
{
  std::shared_ptr<detail::datasource> ds(m_allocator->allocate(false));
  ds->select(tbl);
  return ds;
}

template <>
inline std::shared_ptr<rowset> connection<true>::select(const table_definition& tbl)
{
  return std::make_shared<detail::threaded_rowset>(m_allocator, tbl);
}

template <bool Threading>
table_definition connection<Threading>::fit_to_create(const table_definition& tbl)
{
  using namespace std;

  table_definition res;
  res.id.name = m_fit_identifier.empty()? tbl.id.name: m_fit_identifier;
  for (auto col_iter(begin(tbl.columns)); col_iter != end(tbl.columns); ++col_iter)
  {
    column_definition col;
    col.type = col_iter->type;
    switch (col.type)
    {
    case Geometry:
      col.name = detail::lib::singleton().ogr_geom_wkb();
      col.epsg = col_iter->epsg;
      col.proj = col_iter->proj;
      break;
    default:
      col.name = col_iter->name;
      break;
    }
    res.columns.push_back(col);
  }
  return res;
}

template <bool Threading>
void connection<Threading>::create(const table_definition& tbl)
{
  std::unique_ptr<detail::datasource> ds(m_allocator->allocate(true));
  ds->create(tbl);
}

template <bool Threading>
void connection<Threading>::drop(const table_definition& tbl)
{
  std::unique_ptr<detail::datasource> ds(m_allocator->allocate(true));
  ds->drop(tbl);
}

template <bool Threading>
std::shared_ptr<inserter> connection<Threading>::get_inserter(const table_definition& tbl)
{
  return std::shared_ptr<inserter>(new detail::inserter(m_allocator->allocate(true), tbl));
} // connection<Threading>::

} } // brig::gdal

#endif // BRIG_GDAL_CONNECTION_HPP
