// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_LINK_HPP
#define BRIG_DATABASE_DETAIL_LINK_HPP

#include <boost/algorithm/string.hpp>
#include <boost/utility.hpp>
#include <brig/database/column_detail.hpp>
#include <brig/database/detail/is_geometry_type.hpp>
#include <brig/database/detail/rowset.hpp>
#include <brig/database/detail/sql_identifier.hpp>
#include <brig/database/global.hpp>
#include <brig/database/variant.hpp>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

struct link : public rowset
{
  // dialect
  virtual DBMS system() = 0;
  virtual void sql_parameter(size_t order, const column_detail& param_col, std::ostringstream& stream);
  virtual void sql_column(const column_detail& col, std::ostringstream& stream);

  // command
  virtual void exec
    ( const std::string& sql
    , const std::vector<variant>& params = std::vector<variant>()
    , const std::vector<column_detail>& param_cols = std::vector<column_detail>()
    ) = 0;
  virtual size_t affected() = 0;
  virtual void columns(std::vector<std::string>& cols) = 0;

  // transaction
  virtual void start() = 0;
  virtual void commit() = 0;
  virtual void rollback() = 0;
}; // link

inline void link::sql_parameter(size_t, const column_detail& param_col, std::ostringstream& stream)
{
  using namespace boost::algorithm;
  auto loc = std::locale::classic();
  const DBMS sys(system());
  if (detail::is_geometry_type(sys, param_col))
    switch (sys)
    {
    case UnknownSystem:
      break;

    case DB2:
      stream << param_col.type.schema << '.' << param_col.type.name << "(CAST(? AS BLOB (100M)), " << param_col.srid << ')';
      return;

    case MS_SQL:
      stream << param_col.type.name << "::STGeomFromWKB(?, " << param_col.srid << ')';
      return;

    case MySQL:
    case SQLite:
      stream << "GeomFromWKB(?, " << param_col.srid << ')';
      return;

    case Oracle:
      {
      const bool conv(!iequals(param_col.type.name, "SDO_GEOMETRY", loc));
      if (conv) stream << param_col.type.schema << '.' << param_col.type.name << '(';
      stream << "MDSYS.SDO_GEOMETRY(TO_BLOB(?), " << param_col.srid << ')';
      if (conv) stream << ')';
      }
      return;

    case Postgres:
      if (iequals(param_col.type.name, "GEOGRAPHY", loc))
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
  if (detail::is_geometry_type(sys, col))
    switch (sys)
    {
    case UnknownSystem:
      break;

    case DB2:
      stream << "DB2GSE.ST_AsBinary(";
      detail::sql_identifier(sys, col.name, stream);
      stream << ')';
      return;

    case MS_SQL:
      detail::sql_identifier(sys, col.name, stream);
      stream << ".STAsBinary() ";
      detail::sql_identifier(sys, col.name, stream);
      return;

    case MySQL:
    case SQLite:
      stream << "AsBinary(";
      detail::sql_identifier(sys, col.name, stream);
      stream << ')';
      return;

    case Oracle:
      stream << col.type.schema << '.' << col.type.name << ".GET_WKB(";
      detail::sql_identifier(sys, col.name, stream);
      stream << ')';
      return;

    case Postgres:
      stream << "ST_AsBinary(";
      detail::sql_identifier(sys, col.name, stream);
      stream << ')';
      return;
    }
  detail::sql_identifier(sys, col.name, stream);
} // link::

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_LINK_HPP
