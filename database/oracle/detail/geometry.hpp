// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_DETAIL_GEOMETRY_HPP
#define BRIG_DATABASE_ORACLE_DETAIL_GEOMETRY_HPP

#include <brig/database/oracle/detail/lib.hpp>

namespace brig { namespace database { namespace oracle { namespace detail {

struct point_t {
  OCINumber x, y, z;
}; // point_t

struct point_ind {
  OCIInd atomic, x, y, z;
  bool null() const  { return atomic == OCI_IND_NULL; }
  void reset()  { atomic = x = y = z = OCI_IND_NULL; }
}; // point_ind

struct geometry {
  OCINumber gtype, srid;
  point_t point;
  OCIArray *elem_info, *ordinates;
}; // geometry

struct geometry_ind {
  OCIInd atomic, gtype, srid;
  point_ind point;
  OCIInd elem_info, ordinates;
  bool null() const  { return atomic == OCI_IND_NULL; }
  void reset()  { atomic = gtype = srid = elem_info = ordinates = OCI_IND_NULL; point.reset(); }
}; // geometry_ind

} } } } // brig::database::oracle::detail

#endif // BRIG_DATABASE_ORACLE_DETAIL_GEOMETRY_HPP
