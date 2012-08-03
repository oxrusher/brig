// Andrew Naplavkov

#ifndef BRIG_DATABASE_COMMAND_HPP
#define BRIG_DATABASE_COMMAND_HPP

#include <brig/database/column_definition.hpp>
#include <brig/database/detail/sql_identifier.hpp>
#include <brig/database/global.hpp>
#include <brig/database/rowset.hpp>
#include <brig/database/variant.hpp>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace brig { namespace database {

struct command : public rowset {
  virtual void exec(const std::string& sql, const std::vector<column_definition>& params = std::vector<column_definition>()) = 0;
  virtual size_t affected() = 0;

  // dialect
  virtual DBMS system() = 0;
  virtual std::string sql_parameter(size_t order, const column_definition& param);
  virtual std::string sql_column(const column_definition& col);

  // transaction
  virtual void set_autocommit(bool autocommit) = 0;
  virtual void commit() = 0;
}; // command

inline std::string command::sql_parameter(size_t, const column_definition& param)
{
  using namespace detail;
  const DBMS sys(system());
  std::ostringstream stream; stream.imbue(std::locale::classic());
  if (Geometry == param.type)
    switch (sys)
    {
    default: break;

    case DB2:
      stream << sql_identifier(sys, param.dbms_type) << "(CAST(? AS BLOB (100M)), " << param.srid << ")";
      return stream.str();

    case Informix:
           if ( param.dbms_type_lcase.name.compare("st_geometry") == 0
             || param.dbms_type_lcase.name.compare("st_geomcollection") == 0 ) stream << "ST_GeomFromWKB";
      else if ( param.dbms_type_lcase.name.compare("st_point") == 0 ) stream << "ST_PointFromWKB";
      else if ( param.dbms_type_lcase.name.compare("st_curve") == 0
             || param.dbms_type_lcase.name.compare("st_linestring") == 0 ) stream << "ST_LineFromWKB";
      else if ( param.dbms_type_lcase.name.compare("st_surface") == 0
             || param.dbms_type_lcase.name.compare("st_polygon") == 0 ) stream << "ST_PolyFromWKB";
      else if ( param.dbms_type_lcase.name.compare("st_multipoint") == 0 ) stream << "ST_MPointFromWKB";
      else if ( param.dbms_type_lcase.name.compare("st_multicurve") == 0
             || param.dbms_type_lcase.name.compare("st_multilinestring") == 0 ) stream << "ST_MLineFromWKB";
      else if ( param.dbms_type_lcase.name.compare("st_multisurface") == 0
             || param.dbms_type_lcase.name.compare("st_multipolygon") == 0 ) stream << "ST_MPolyFromWKB";
      else throw std::runtime_error("type error");
      stream << "(?, " << param.srid << ")";
      return stream.str();

    case MS_SQL:
      stream << sql_identifier(sys, param.dbms_type) << "::STGeomFromWKB(?, " << param.srid << ").MakeValid()";
      return stream.str();

    case MySQL:
    case SQLite:
      stream << "GeomFromWKB(?, " << param.srid << ")";
      return stream.str();

    case Oracle:
      {
      const bool conv(param.dbms_type_lcase.name.compare("sdo_geometry") != 0);
      if (conv) stream << sql_identifier(sys, param.dbms_type) << "(";
      stream << "MDSYS.SDO_GEOMETRY(TO_BLOB(?), " << param.srid << ")";
      if (conv) stream << ")";
      }
      return stream.str();

    case Postgres:
      if (param.dbms_type_lcase.name.compare("geography") == 0)
      {
        if (param.srid != 4326) throw std::runtime_error("SRID error");
        stream << "ST_GeogFromWKB(?)";
      }
      else
        stream << "ST_GeomFromWKB(?, " << param.srid << ")";
      return stream.str();
    }
  return "?";
}

inline std::string command::sql_column(const column_definition& col)
{
  using namespace detail;
  const DBMS sys(system());
  const std::string id(sql_identifier(sys, col.name));

  if (!col.query_expression.empty()) return col.query_expression + " as " + id;

  // http://en.wikipedia.org/wiki/ISO_8601
  if (String == col.type)
  {
    if (col.dbms_type_lcase.name.find("time") != std::string::npos)
      switch (sys)
      {
      default: break;
      case CUBRID:
      case DB2:
      case Oracle:
      case Postgres: return "(TO_CHAR(" + id + ", 'YYYY-MM-DD') || 'T' || TO_CHAR(" + id + ", 'HH24:MI:SS')) as " + id;
      case MS_SQL: return "CONVERT(CHAR(19), " + id + ", 126) as " + id;
      case MySQL: return "DATE_FORMAT(" + id + ", '%Y-%m-%dT%T') as " + id;
      }
    else if (col.dbms_type_lcase.name.find("date") != std::string::npos)
      switch (sys)
      {
      default: break;
      case CUBRID:
      case DB2:
      case Oracle:
      case Postgres: return "TO_CHAR(" + id + ", 'YYYY-MM-DD') as " + id;
      case MS_SQL: return "CONVERT(CHAR(10), " + id + ", 126) as " + id;
      case MySQL: return "DATE_FORMAT(" + id + ", '%Y-%m-%d') as " + id;
      }
  }

  if (Postgres == sys)
  {
    if (VoidColumn == col.type) return id;
    else if (col.dbms_type_lcase.name.compare("raster") == 0) return "ST_AsBinary(ST_Envelope(" + id + ")) as " + id;
    else if (col.dbms_type_lcase.name.compare("geography") == 0
          || col.dbms_type_lcase.name.compare("geometry") == 0) return "ST_AsBinary(" + id + ") as " + id;
    else return id;
  }

  if (Geometry == col.type)
    switch (sys)
    {
    default: break;
    case DB2: return "DB2GSE.ST_AsBinary(" + id + ") as " + id;
    case Informix: return "ST_AsBinary(" + id + ") as " + id;
    case MS_SQL: return id + ".STAsBinary() as " + id;
    case MySQL:
    case SQLite: return "AsBinary(" + id + ") as " + id;
    case Oracle: return sql_identifier(sys, col.dbms_type) + ".GET_WKB(" + id + ") as " + id;
    }

  return id;
} // command::

} } // brig::database

#endif // BRIG_DATABASE_COMMAND_HPP
