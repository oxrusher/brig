// Andrew Naplavkov

#ifndef BRIG_DATABASE_COMMAND_HPP
#define BRIG_DATABASE_COMMAND_HPP

#include <brig/database/column_detail.hpp>
#include <brig/database/detail/is_geometry_type.hpp>
#include <brig/database/detail/sql_identifier.hpp>
#include <brig/database/detail/sql_object.hpp>
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
  virtual void exec
    ( const std::string& sql
    , const std::vector<variant>& params = std::vector<variant>()
    , const std::vector<column_detail>& param_cols = std::vector<column_detail>()
    ) = 0;
  virtual size_t affected() = 0;

  // dialect
  virtual DBMS system() = 0;
  virtual std::string sql_parameter(size_t order, const column_detail& param_col);
  virtual std::string sql_column(const column_detail& col);

  // transaction
  virtual void set_autocommit(bool autocommit) = 0;
  virtual void commit() = 0;
}; // command

inline std::string command::sql_parameter(size_t, const column_detail& param_col)
{
  using namespace detail;
  const DBMS sys(system());
  std::ostringstream stream; stream.imbue(std::locale::classic());
  if (is_geometry_type(sys, param_col))
    switch (sys)
    {
    case DB2:
      stream << sql_object(sys, param_col.type) << "(CAST(? AS BLOB (100M)), " << param_col.srid << ")";
      return stream.str();

    case MS_SQL:
      stream << sql_object(sys, param_col.type) << "::STGeomFromWKB(?, " << param_col.srid << ")";
      return stream.str();

    case MySQL:
    case SQLite:
      stream << "GeomFromWKB(?, " << param_col.srid << ")";
      return stream.str();

    case Oracle:
      {
      const bool conv("sdo_geometry" != param_col.lower_case_type.name);
      if (conv) stream << sql_object(sys, param_col.type) << "(";
      stream << "MDSYS.SDO_GEOMETRY(TO_BLOB(?), " << param_col.srid << ")";
      if (conv) stream << ")";
      }
      return stream.str();

    case Postgres:
      if ("geography" == param_col.lower_case_type.name)
      {
        if (param_col.srid != 4326) throw std::runtime_error("it only supports wgs 84 long lat (srid:4326)");
        stream << "ST_GeogFromWKB(?)";
      }
      else
        stream << "ST_GeomFromWKB(?, " << param_col.srid << ")";
      return stream.str();
    }
  return "?";
}

inline std::string command::sql_column(const column_detail& col)
{
  using namespace detail;
  const DBMS sys(system());
  if (is_geometry_type(sys, col))
    switch (sys)
    {
    case DB2: return "DB2GSE.ST_AsBinary(" + sql_identifier(sys, col.name) + ")";
    case MS_SQL: return sql_identifier(sys, col.name) + ".STAsBinary() " + sql_identifier(sys, col.name);
    case MySQL:
    case SQLite: return "AsBinary(" + sql_identifier(sys, col.name) + ")";
    case Oracle: return sql_object(sys, col.type) + ".GET_WKB(" + sql_identifier(sys, col.name) + ")";
    case Postgres: return "ST_AsBinary(" + sql_identifier(sys, col.name) + ")";
    }
  return sql_identifier(sys, col.name);
} // command::

} } // brig::database

#endif // BRIG_DATABASE_COMMAND_HPP
