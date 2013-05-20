// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_DETAIL_BINDING_GEOMETRY_HPP
#define BRIG_DATABASE_ORACLE_DETAIL_BINDING_GEOMETRY_HPP

#include <brig/blob_t.hpp>
#include <brig/database/oracle/detail/binding.hpp>
#include <brig/database/oracle/detail/handles.hpp>
#include <brig/database/oracle/detail/geometry.hpp>
#include <brig/database/oracle/detail/lib.hpp>
#include <brig/detail/ogc.hpp>
#include <cstdint>
#include <stdexcept>

namespace brig { namespace database { namespace oracle { namespace detail {

class binding_geometry : public binding
{
  handles* m_hnd;
  geometry* m_geom;
  geometry_ind* m_ind;

  void free();
  void add_real(double val, OCIColl* coll) const;
  void add_info(uint32_t starting_offset, uint32_t etype, uint32_t interpretation) const;
  template <typename InputIterator> void add_point(uint8_t byte_order, InputIterator& itr, uint32_t& offset) const;
  template <typename InputIterator> void add_line(uint8_t byte_order, InputIterator& itr, uint32_t& offset, uint32_t etype) const;
  template <typename InputIterator> void add_polygon(uint8_t byte_order, InputIterator& itr, uint32_t& offset) const;
  template <typename InputIterator> void add_geom(InputIterator& itr, uint32_t& offset) const;

public:
  binding_geometry(handles* hnd, size_t order, const blob_t& blob, int srid);
  ~binding_geometry() override  { free(); }
}; // binding_geometry

inline void binding_geometry::free()
{
  if (m_geom) lib::singleton().p_OCIObjectFree(m_hnd->env, m_hnd->err, m_geom, OCI_OBJECTFREE_FORCE);
  delete m_ind;
}

inline void binding_geometry::add_real(double val, OCIColl* coll) const
{
  OCINumber num;
  m_hnd->set_real(val, &num);
  m_hnd->check(lib::singleton().p_OCICollAppend(m_hnd->env, m_hnd->err, &num, 0, coll));
}

inline void binding_geometry::add_info(uint32_t starting_offset, uint32_t etype, uint32_t interpretation) const
{
  OCINumber num;
  m_hnd->set_int(starting_offset, &num);
  m_hnd->check(lib::singleton().p_OCICollAppend(m_hnd->env, m_hnd->err, &num, 0, m_geom->elem_info));
  m_hnd->set_int(etype, &num);
  m_hnd->check(lib::singleton().p_OCICollAppend(m_hnd->env, m_hnd->err, &num, 0, m_geom->elem_info));
  m_hnd->set_int(interpretation, &num);
  m_hnd->check(lib::singleton().p_OCICollAppend(m_hnd->env, m_hnd->err, &num, 0, m_geom->elem_info));
  m_ind->elem_info = OCI_IND_NOTNULL;
}

template <typename InputIterator>
void binding_geometry::add_point(uint8_t byte_order, InputIterator& itr, uint32_t& offset) const
{
  using namespace brig::detail::ogc;

  add_real(read<double>(byte_order, itr), m_geom->ordinates);
  add_real(read<double>(byte_order, itr), m_geom->ordinates);
  m_ind->ordinates = OCI_IND_NOTNULL;
  offset += 2;
}

template <typename InputIterator>
void binding_geometry::add_line(uint8_t byte_order, InputIterator& itr, uint32_t& offset, uint32_t etype) const
{
  add_info(offset, etype, 1);
  for (uint32_t i(0), count(brig::detail::ogc::read<uint32_t>(byte_order, itr)); i < count; ++i)
    add_point(byte_order, itr, offset);
}

template <typename InputIterator>
void binding_geometry::add_polygon(uint8_t byte_order, InputIterator& itr, uint32_t& offset) const
{
  for (uint32_t i(0), count(brig::detail::ogc::read<uint32_t>(byte_order, itr)); i < count; ++i)
    add_line(byte_order, itr, offset, i == 0? 1003: 2003);
}

template <typename InputIterator>
void binding_geometry::add_geom(InputIterator& itr, uint32_t& offset) const
{
  using namespace std;
  using namespace brig::detail::ogc;

  uint8_t byte_order(read_byte_order(itr));
  uint32_t i(0), count(0);
  switch (read<uint32_t>(byte_order, itr)) // type
  {
  default: throw runtime_error("WKB error");

  case Point:
    add_info(offset, 1, 1);
    add_point(byte_order, itr, offset);
    break;

  case LineString:
    add_line(byte_order, itr, offset, 2);
    break;

  case Polygon:
    add_polygon(byte_order, itr, offset);
    break;

  case MultiPoint:
    count = read<uint32_t>(byte_order, itr);
    add_info(offset, 1, count);
    for (i = 0; i < count; ++i)
    {
      byte_order = read_byte_order(itr);
      if (Point != read<uint32_t>(byte_order, itr)) throw runtime_error("WKB error");
      add_point(byte_order, itr, offset);
    }
    break;

  case MultiLineString:
    for (i = 0, count = read<uint32_t>(byte_order, itr); i < count; ++i)
    {
      byte_order = read_byte_order(itr);
      if (LineString != read<uint32_t>(byte_order, itr)) throw runtime_error("WKB error");
      add_line(byte_order, itr, offset, 2);
    }
    break;

  case MultiPolygon:
    for (i = 0, count = read<uint32_t>(byte_order, itr); i < count; ++i)
    {
      byte_order = read_byte_order(itr);
      if (Polygon != read<uint32_t>(byte_order, itr)) throw runtime_error("WKB error");
      add_polygon(byte_order, itr, offset);
    }
    break;

  case GeometryCollection:
    for (i = 0, count = read<uint32_t>(byte_order, itr); i < count; ++i)
      add_geom(itr, offset);
    break;
  }
}

inline binding_geometry::binding_geometry(handles* hnd, size_t order, const blob_t& blob, int srid) : m_hnd(hnd), m_geom(0), m_ind(0)
{
  using namespace std;
  using namespace brig::detail::ogc;

  m_hnd->check(lib::singleton().p_OCIObjectNew(m_hnd->env, m_hnd->err, m_hnd->svc, OCI_TYPECODE_OBJECT, m_hnd->geom, 0, OCI_DURATION_DEFAULT, true, (void**)&m_geom));
  try
  {
    m_ind = new geometry_ind(); m_ind->reset();
    OCIBind* bnd(0);
    m_hnd->check(lib::singleton().p_OCIBindByPos(m_hnd->stmt, &bnd, m_hnd->err, ub4(order), 0, 0, SQLT_NTY, 0, 0, 0, 0, 0, OCI_DEFAULT));
    m_hnd->check(lib::singleton().p_OCIBindObject(bnd, m_hnd->err, m_hnd->geom, (void**)&m_geom, 0, (void**)&m_ind, 0));
    
    if (0 == blob.size()) return;
    m_ind->atomic = OCI_IND_NOTNULL;
    if (srid > 0) m_hnd->set_int(srid, &m_geom->srid, &m_ind->srid);

    auto ptr = blob.data();
    uint8_t byte_order(read_byte_order(ptr));
    switch (read<uint32_t>(byte_order, ptr)) // type
    {
    default: throw runtime_error("WKB error");
    case Point:
      m_hnd->set_int(2001, &m_geom->gtype, &m_ind->gtype);
      m_hnd->set_real(read<double>(byte_order, ptr), &m_geom->point.x, &m_ind->point.x);
      m_hnd->set_real(read<double>(byte_order, ptr), &m_geom->point.y, &m_ind->point.y);
      return;
    case LineString: m_hnd->set_int(2002, &m_geom->gtype, &m_ind->gtype); break;
    case Polygon: m_hnd->set_int(2003, &m_geom->gtype, &m_ind->gtype); break;
    case MultiPoint: m_hnd->set_int(2005, &m_geom->gtype, &m_ind->gtype); break;
    case MultiLineString: m_hnd->set_int(2006, &m_geom->gtype, &m_ind->gtype); break;
    case MultiPolygon: m_hnd->set_int(2007, &m_geom->gtype, &m_ind->gtype); break;
    case GeometryCollection: m_hnd->set_int(2004, &m_geom->gtype, &m_ind->gtype); break;
    }

    ptr = blob.data();
    uint32_t offset(1);
    add_geom(ptr, offset);
  }
  catch (const exception&)  { free(); throw; }
} // binding_geometry::

} } } } // brig::database::oracle::detail

#endif // BRIG_DATABASE_ORACLE_DETAIL_BINDING_GEOMETRY_HPP
