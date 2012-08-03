// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_INTERSECT_HPP
#define BRIG_DATABASE_DETAIL_SQL_INTERSECT_HPP

#include <brig/boost/geometry.hpp>
#include <brig/database/column_definition.hpp>
#include <brig/database/detail/sql_identifier.hpp>
#include <brig/database/global.hpp>
#include <ios>
#include <locale>
#include <stdexcept>
#include <sstream>
#include <string>

namespace brig { namespace database { namespace detail {

inline std::string sql_intersect(DBMS sys, const column_definition& col, const boost::box& box)
{
  const std::string id(sql_identifier(sys, col.name));
  const double xmin(box.min_corner().get<0>()), ymin(box.min_corner().get<1>()), xmax(box.max_corner().get<0>()), ymax(box.max_corner().get<1>());
  std::ostringstream stream; stream.imbue(std::locale::classic()); stream << std::scientific; stream.precision(17);

  if (Postgres == sys)
  {
    bool geography(false), raster(false);
    if (VoidColumn == col.type) throw std::runtime_error("SQL error");
    else if (col.dbms_type_lcase.name.compare("raster") == 0) raster = true;
    else if (col.dbms_type_lcase.name.compare("geography") == 0) geography = true;
    else if (col.dbms_type_lcase.name.compare("geometry") != 0) throw std::runtime_error("SQL error");
    if (geography && col.srid != 4326) throw std::runtime_error("SRID error");

    if (raster) stream << "ST_Envelope(";
    stream << id;
    if (raster) stream << ")";
    stream << " && ";
    if (geography) stream << "ST_GeogFromWKB(ST_AsBinary(";
    stream << "ST_SetSRID(ST_MakeBox2D(ST_Point(" << xmin << ", " << ymin << "), ST_Point(" << xmax << ", " << ymax << ")), " << col.srid << ")";
    if (geography) stream << "))";
  }

  else if (Geometry != col.type)
    throw std::runtime_error("SQL error");

  else switch (sys)
  {
  default: throw std::runtime_error("SQL error");
  case DB2: stream << "DB2GSE.EnvelopesIntersect(" << id << ", " << xmin << ", " << ymin << ", " << xmax << ", " << ymax << ", " << col.srid << ") = 1"; break;
  case Informix: stream << "SE_EnvelopesIntersect(" << id << ", ST_Envelope(ST_Union(ST_Point(" << xmin << ", " << ymin << ", " << col.srid << "), ST_Point(" << xmax << ", " << ymax << ", " << col.srid << "))))"; break;
  case MS_SQL:
    {
    const bool geography(col.dbms_type_lcase.name.compare("geography") == 0);
    stream << "" << id << ".Filter(";
    if (geography) stream << "geography::STGeomFromWKB(";
    stream << "geometry::Point(" << xmin << ", " << ymin << ", " << col.srid << ").STUnion(geometry::Point(" << xmax << ", " << ymax << ", " << col.srid << ")).STEnvelope()";
    if (geography) stream << ".STAsBinary(), " << col.srid << ")";
    stream << ") = 1";
    }
    break;
  case MySQL: stream << "MBRIntersects(Envelope(LineString(Point(" << xmin << ", " << ymin << "), Point(" << xmax << ", " << ymax << "))), " << id << ")"; break;
  case Oracle: stream << "MDSYS.SDO_FILTER(" << id << ", MDSYS.SDO_GEOMETRY(2003, " << col.srid << ", NULL, MDSYS.SDO_ELEM_INFO_ARRAY(1, 1003, 3), MDSYS.SDO_ORDINATE_ARRAY(" << xmin << ", " << ymin << ", " << xmax << ", " << ymax << "))) = 'TRUE'"; break;
  case SQLite: stream << "xmax >= " << xmin << " AND xmin <= " << xmax << " AND ymax >= " << ymin << " AND ymin <= " << ymax; break;
  }

  return stream.str();
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_INTERSECT_HPP
