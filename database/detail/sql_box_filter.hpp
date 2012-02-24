// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_BOX_FILTER_HPP
#define BRIG_DATABASE_DETAIL_SQL_BOX_FILTER_HPP

#include <brig/boost/geometry.hpp>
#include <brig/database/column_detail.hpp>
#include <brig/database/detail/is_geometry_type.hpp>
#include <brig/database/detail/sql_identifier.hpp>
#include <brig/database/global.hpp>
#include <locale>
#include <stdexcept>
#include <sstream>
#include <string>

namespace brig { namespace database { namespace detail {

inline std::string sql_box_filter(DBMS sys, const column_detail& col, const boost::box& box)
{
  const std::string id(sql_identifier(sys, col.name));
  const double xmin(box.min_corner().get<0>()), ymin(box.min_corner().get<1>()), xmax(box.max_corner().get<0>()), ymax(box.max_corner().get<1>());
  std::ostringstream stream; stream.imbue(std::locale::classic());

  if (Postgres == sys)
  {
    bool geography(false), raster(false);

    if ("user-defined" != col.lower_case_type.schema) throw std::runtime_error("SQL error");
    else if ("raster" == col.lower_case_type.name) raster = true;
    else if ("geography" == col.lower_case_type.name) geography = true;
    else if ("geometry" != col.lower_case_type.name) throw std::runtime_error("SQL error");

    if (raster) stream << "ST_Envelope(";
    stream << id;
    if (raster) stream << ")";
    stream << " && ";
    if (geography) stream << "ST_GeogFromWKB(ST_AsBinary(";
    if (0 < col.srid) stream << "ST_SetSRID(";
    stream << "ST_MakeBox2D(ST_Point(" << xmin << ", " << ymin << "), ST_Point(" << xmax << ", " << ymax << "))";
    if (0 < col.srid) stream << ", " << col.srid << ")";
    if (geography) stream << "))";
  }

  else if (!is_geometry_type(sys, col))
    throw std::runtime_error("SQL error");

  else switch (sys)
  {
  default:
    throw std::runtime_error("SQL error");
  case DB2:
    stream << "DB2GSE.EnvelopesIntersect(" << id << ", " << xmin << ", " << ymin << ", " << xmax << ", " << ymax << ", " << (0 < col.srid? col.srid: 0) << ") = 1";
    break;
  case MS_SQL:
    {
    const bool geography("geography" == col.lower_case_type.name);
    const int srid(0 < col.srid? col.srid: geography? 4326: 0);
    stream << "" << id << ".Filter(";
    if (geography) stream << "GEOGRAPHY::STGeomFromWKB(";
    stream << "GEOMETRY::Point(" << xmin << ", " << ymin << ", " << srid << ").STUnion(GEOMETRY::Point(" << xmax << ", " << ymax << ", " << srid << ")).STEnvelope()";
    if (geography) stream << ".STAsBinary(), " << srid << ")";
    stream << ") = 1";
    }
    break;
  case MySQL:
    stream << "MBRIntersects(Envelope(LineString(Point(" << xmin << ", " << ymin << "), Point(" << xmax << ", " << ymax << "))), " << id << ")";
    break;
  case Oracle:
    stream << "MDSYS.SDO_FILTER(" << id << ", MDSYS.SDO_GEOMETRY(2003, ";
    if (0 < col.srid) stream << col.srid;
    else stream << "NULL";
    stream << ", NULL, SDO_ELEM_INFO_ARRAY(1, 1003, 3), SDO_ORDINATE_ARRAY(" << xmin << ", " << ymin << ", " << xmax << ", " << ymax << "))) = 'TRUE'";
    break;
  case SQLite:
    stream << "xmax >= " << xmin << " AND xmin <= " << xmax << " AND ymax >= " << ymin << " AND ymin <= " << ymax;
    break;
  }

  return stream.str();
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_BOX_FILTER_HPP
