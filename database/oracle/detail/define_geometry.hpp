// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_DETAIL_DEFINE_GEOMETRY_HPP
#define BRIG_DATABASE_ORACLE_DETAIL_DEFINE_GEOMETRY_HPP

#include <brig/blob_t.hpp>
#include <brig/database/oracle/detail/define.hpp>
#include <brig/database/oracle/detail/geometry.hpp>
#include <brig/database/oracle/detail/handles.hpp>
#include <brig/database/oracle/detail/lib.hpp>
#include <brig/detail/back_insert_iterator.hpp>
#include <brig/detail/ogc.hpp>
#include <brig/variant.hpp>
#include <cstdint>
#include <stdexcept>

namespace brig { namespace database { namespace oracle { namespace detail {

class define_geometry : public define {
  handles* m_hnd;
  geometry* m_geom;
  geometry_ind* m_ind;

  uint32_t starting_offset(uint32_t info_beg) const  { return m_hnd->get_int(m_hnd->get_number(m_geom->elem_info, info_beg)) - 1; }
  uint32_t etype(uint32_t info_beg) const  { return m_hnd->get_int(m_hnd->get_number(m_geom->elem_info, info_beg + 1)); }
  uint32_t interpretation(uint32_t info_beg) const  { return m_hnd->get_int(m_hnd->get_number(m_geom->elem_info, info_beg + 2)); }
  template <typename OutputIterator> void set_point(OutputIterator& iter, uint32_t ord_beg) const;
  template <typename OutputIterator> void set_line(OutputIterator& iter, uint32_t ord_beg, uint32_t ord_end, uint32_t dim) const;

public:
  define_geometry(handles* hnd, size_t order);
  ~define_geometry() override;
  void operator()(variant& var) override;
}; // define_geometry

inline define_geometry::define_geometry(handles* hnd, size_t order) : m_hnd(hnd), m_geom(0), m_ind(0)
{
  OCIDefine* def(0);
  m_hnd->check(lib::singleton().p_OCIDefineByPos(m_hnd->stmt, &def, m_hnd->err, ub4(order), 0, 0, SQLT_NTY, 0, 0, 0, OCI_DEFAULT));
  m_hnd->check(lib::singleton().p_OCIDefineObject(def, m_hnd->err, m_hnd->geom, (void**)&m_geom, 0, (void**)&m_ind, 0));
}

inline define_geometry::~define_geometry()
{
  lib::singleton().p_OCIObjectFree(m_hnd->env, m_hnd->err, m_geom, OCI_OBJECTFREE_FORCE);
}

template <typename OutputIterator>
void define_geometry::set_point(OutputIterator& iter, uint32_t ord_beg) const
{
  using namespace brig::detail::ogc;

  write<double>(iter, m_hnd->get_real(m_hnd->get_number(m_geom->ordinates, ord_beg)));
  write<double>(iter, m_hnd->get_real(m_hnd->get_number(m_geom->ordinates, ord_beg + 1)));
}

template <typename OutputIterator>
void define_geometry::set_line(OutputIterator& iter, uint32_t ord_beg, uint32_t ord_end, uint32_t dim) const
{
  brig::detail::ogc::write<uint32_t>(iter, (ord_end - ord_beg) / dim); // numPoints
  for (uint32_t ord(ord_beg); ord < ord_end; ord += dim)
    set_point(iter, ord);
}

inline void define_geometry::operator()(variant& var)
{
  using namespace std;
  using namespace brig::detail::ogc;

  if (!m_ind || m_ind->null())  { var = null_t(); return; }
  const uint32_t gtype (m_hnd->get_int(&(m_geom->gtype), m_ind->gtype));
  const uint32_t tt    (gtype % 100);
  const uint32_t dim   (gtype / 1000);
  if (2 != dim) throw runtime_error("OCI geometry error");
  const uint32_t infos (m_hnd->get_count(m_geom->elem_info, m_ind->elem_info));
  const uint32_t ords  (m_hnd->get_count(m_geom->ordinates, m_ind->ordinates));
  bool collection(false);
  switch (tt)
  {
  default: throw runtime_error("OCI geometry error");
  case 1: // point
  case 2: // line / curve
  case 3: // polygon / surface
    break;
  case 4: // collection
  case 5: // multipoint
  case 6: // multiline
  case 7: // multipolygon / multisurface
    collection = true;
    break;
  }
  uint32_t num_geoms(0);

  var = blob_t();
  blob_t& blob = ::boost::get<blob_t>(var);
  const size_t header_size(sizeof(uint8_t) + 2 * sizeof(uint32_t));
  if (collection) blob.resize(header_size);
  blob.reserve((collection? 3: 1) * header_size + ((ords / dim) * 2 * sizeof(double)) ); // estimate size
  auto iter = brig::detail::back_inserter(blob);

  if (0 == infos)
  {
    write_byte_order(iter);
    write<uint32_t>(iter, Point);
    write<double>(iter, m_hnd->get_real(&m_geom->point.x, m_ind->point.x));
    write<double>(iter, m_hnd->get_real(&m_geom->point.y, m_ind->point.y));

    ++num_geoms;
  }

  uint32_t i(0);
  while (i < infos)
  {
    switch (etype(i))
    {
    default: throw runtime_error("OCI geometry error");

    case 1: // point / cluster
      {
        const uint32_t num_points(interpretation(i));
        if (num_points < 1) throw runtime_error("OCI geometry error"); // oriented point
        const uint32_t ord_beg (starting_offset(i));
        const uint32_t ord_end (ord_beg + num_points * dim);
        for (uint32_t ord(ord_beg); ord < ord_end; ord += dim)
        {
          write_byte_order(iter);
          write<uint32_t>(iter, Point);
          set_point(iter, ord);
        }

        num_geoms += num_points;
        i += 3; // triplet
      }
      break;

    case 2: // line string
      {
        if (1 != interpretation(i)) throw runtime_error("OCI geometry error"); // not straight line segments
        write_byte_order(iter);
        write<uint32_t>(iter, LineString);
        set_line(iter, starting_offset(i), i + 3 < infos? starting_offset(i + 3): ords, dim); // triplet

        ++num_geoms;
        i += 3; // triplet
      }
      break;

    case 1003: // exterior polygon ring
      {
        uint32_t num_rings(1);
        for (uint32_t j(i + 3); j < infos; j += 3) // triplet
        {
          if (2003 != etype(j)) break; // not interior polygon ring
          ++num_rings;
        }

        write_byte_order(iter);
        write<uint32_t>(iter, Polygon);
        write<uint32_t>(iter, num_rings);

        const uint32_t info_end(i + num_rings * 3); // triplet
        for (; i < info_end; i += 3) // triplet
        {
          const uint32_t ord_beg(starting_offset(i));
          switch (interpretation(i))
          {
          default: throw runtime_error("OCI geometry error");

          case 1: // simple polygon
            set_line(iter, ord_beg, i + 3 < infos? starting_offset(i + 3): ords, dim); // triplet
            break;

          case 3: // optimized rectangle
            {
              const double left  (m_hnd->get_real(m_hnd->get_number(m_geom->ordinates, ord_beg)));
              const double lower (m_hnd->get_real(m_hnd->get_number(m_geom->ordinates, ord_beg + 1)));
              const double right (m_hnd->get_real(m_hnd->get_number(m_geom->ordinates, ord_beg + dim)));
              const double upper (m_hnd->get_real(m_hnd->get_number(m_geom->ordinates, ord_beg + dim + 1)));

              write<uint32_t>(iter, 5); // numPoints
              write<double>(iter, left); write<double>(iter, lower);
              if (i == i) // exterior ring
              {
                write<double>(iter, right); write<double>(iter, lower);
                write<double>(iter, right); write<double>(iter, upper);
                write<double>(iter, left); write<double>(iter, upper);
              }
              else // interior ring
              {
                write<double>(iter, left); write<double>(iter, upper);
                write<double>(iter, right); write<double>(iter, upper);
                write<double>(iter, right); write<double>(iter, lower);
              }
              write<double>(iter, left); write<double>(iter, lower);
            }
            break;
          }
        }

        ++num_geoms;
      }
      break;
    }
  }

  if (collection)
  {
    auto ptr = blob.data();
    write_byte_order(ptr);
    switch (tt)
    {
    default: throw runtime_error("OCI geometry error");
    case 4: write<uint32_t>(ptr, GeometryCollection); break; // collection
    case 5: write<uint32_t>(ptr, MultiPoint); break; // multipoint
    case 6: write<uint32_t>(ptr, MultiLineString); break; // multiline
    case 7: write<uint32_t>(ptr, MultiPolygon); break; // multipolygon / multisurface
    }
    write<uint32_t>(ptr, num_geoms);
  }
} // define_geometry::

} } } } // brig::database::oracle::detail

#endif // BRIG_DATABASE_ORACLE_DETAIL_DEFINE_GEOMETRY_HPP
