// Andrew Naplavkov

// http://www.gaia-gis.it/gaia-sins/BLOB-Geometry.html

#ifndef BRIG_DATABASE_SQLITE_DETAIL_COLUMN_GEOMETRY_HPP
#define BRIG_DATABASE_SQLITE_DETAIL_COLUMN_GEOMETRY_HPP

#include <brig/database/sqlite/detail/lib.hpp>
#include <brig/detail/ogc.hpp>
#include <cstdint>
#include <stdexcept>

namespace brig { namespace database { namespace sqlite { namespace detail {

inline void skip_point(uint8_t*& ptr)
{
  ptr += 2 * sizeof(double);
}

inline void skip_line(uint8_t byte_order, uint8_t*& ptr)
{
  const uint32_t count(brig::detail::ogc::get<uint32_t>(byte_order, ptr));
  ptr += count * 2 * sizeof(double);
}

inline void skip_polygon(uint8_t byte_order, uint8_t*& ptr)
{
  for (uint32_t i(0), count(brig::detail::ogc::get<uint32_t>(byte_order, ptr)); i < count; ++i)
    skip_line(byte_order, ptr);
}

inline void column_geometry(sqlite3_stmt* stmt, int col, blob_t& blob)
{
  using namespace brig::detail::ogc;

  const uint8_t* data((const uint8_t*)lib::singleton().p_sqlite3_column_blob(stmt, col));
  const int size(lib::singleton().p_sqlite3_column_bytes(stmt, col));
  if (size <= 39) return;

  blob.resize(size - 39);
  const uint8_t byte_order(data[1]);
  blob[0] = byte_order;
  memcpy(blob.data() + 1, data + 39, blob.size() - 1);

  uint8_t* ptr = blob.data() + 1;
  switch (get<uint32_t>(byte_order, ptr))
  {
  default: throw std::runtime_error("SpatiaLite geometry error");
  case Point:
  case LineString:
  case Polygon: return;
  case MultiPoint:
  case MultiLineString:
  case MultiPolygon:
  case GeometryCollection: break;
  }

  for (uint32_t i(0), count(get<uint32_t>(byte_order, ptr)); i < count; ++i)
  {
    set<uint8_t>(ptr, byte_order);
    switch (get<uint32_t>(byte_order, ptr))
    {
    default: throw std::runtime_error("SpatiaLite geometry error");
    case Point: skip_point(ptr); break;
    case LineString: skip_line(byte_order, ptr); break;
    case Polygon: skip_polygon(byte_order, ptr); break;
    }
  }
}

} } } } // brig::database::sqlite::detail

#endif // BRIG_DATABASE_SQLITE_DETAIL_COLUMN_GEOMETRY_HPP
