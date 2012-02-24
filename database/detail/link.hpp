// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_LINK_HPP
#define BRIG_DATABASE_DETAIL_LINK_HPP

#include <brig/database/column_detail.hpp>
#include <brig/database/command.hpp>
#include <brig/database/detail/is_geometry_type.hpp>
#include <brig/database/detail/sql_identifier.hpp>
#include <brig/database/detail/sql_object.hpp>
#include <brig/database/global.hpp>
#include <sstream>
#include <stdexcept>

namespace brig { namespace database { namespace detail {

struct link : public command
{
  // dialect
  virtual DBMS system() = 0;
  virtual void sql_parameter(size_t order, const column_detail& param_col, std::ostringstream& stream);
  virtual void sql_column(const column_detail& col, std::ostringstream& stream);

  // transaction
  virtual void start() = 0;
  virtual void commit() = 0;
  virtual void rollback() = 0;
}; // link

inline void link::sql_parameter(size_t, const column_detail& param_col, std::ostringstream& stream)
{
  const DBMS sys(system());
  if (is_geometry_type(sys, param_col))
    switch (sys)
    {
    case VoidSystem:
      break;

    case DB2:
      stream << sql_object(sys, param_col.type) << "(CAST(? AS BLOB (100M)), " << param_col.srid << ')';
      return;

    case MS_SQL:
      stream << sql_object(sys, param_col.type) << "::STGeomFromWKB(?, " << param_col.srid << ')';
      return;

    case MySQL:
    case SQLite:
      stream << "GeomFromWKB(?, " << param_col.srid << ')';
      return;

    case Oracle:
      {
      const bool conv("sdo_geometry" != param_col.case_folded_type.name);
      if (conv) stream << sql_object(sys, param_col.type) << '(';
      stream << "MDSYS.SDO_GEOMETRY(TO_BLOB(?), " << param_col.srid << ')';
      if (conv) stream << ')';
      }
      return;

    case Postgres:
      if ("geography" == param_col.case_folded_type.name)
      {
        if (param_col.srid != 4326) throw std::runtime_error("it only supports wgs 84 long lat (srid:4326)");
        stream << "ST_GeogFromWKB(?)";
      }
      else
        stream << "ST_GeomFromWKB(?, " << param_col.srid << ')';
      return;
    }
  stream << '?';
}

inline void link::sql_column(const column_detail& col, std::ostringstream& stream)
{
  const DBMS sys(system());
  if (is_geometry_type(sys, col))
    switch (sys)
    {
    case VoidSystem:
      break;

    case DB2:
      stream << "DB2GSE.ST_AsBinary(" << sql_identifier(sys, col.name) << ')';
      return;

    case MS_SQL:
      stream << sql_identifier(sys, col.name) << ".STAsBinary() " << sql_identifier(sys, col.name);
      return;

    case MySQL:
    case SQLite:
      stream << "AsBinary(" << sql_identifier(sys, col.name) << ')';
      return;

    case Oracle:
      stream << sql_object(sys, col.type) << ".GET_WKB(" << sql_identifier(sys, col.name) << ')';
      return;

    case Postgres:
      stream << "ST_AsBinary(" << sql_identifier(sys, col.name) << ')';
      return;
    }
  stream << sql_identifier(sys, col.name);
} // link::

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_LINK_HPP
