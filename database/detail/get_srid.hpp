// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_SRID_HPP
#define BRIG_DATABASE_DETAIL_GET_SRID_HPP

#include <brig/database/command.hpp>
#include <brig/database/global.hpp>
#include <brig/database/numeric_cast.hpp>
#include <brig/database/variant.hpp>
#include <brig/string_cast.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

inline int get_srid(std::shared_ptr<command> cmd, int epsg)
{
  std::string sql;
  switch (cmd->system())
  {
  default:
  case CUBRID: throw std::runtime_error("DBMS error");
  case DB2: sql = "SELECT SRS_ID FROM DB2GSE.ST_SPATIAL_REFERENCE_SYSTEMS WHERE ORGANIZATION LIKE 'EPSG' AND ORGANIZATION_COORDSYS_ID = " + string_cast<char>(epsg) + " ORDER BY SRS_ID FETCH FIRST 1 ROWS ONLY"; break;
  case Informix: sql = "SELECT FIRST 1 srid FROM sde.spatial_references WHERE auth_name = 'EPSG' AND auth_srid = " + string_cast<char>(epsg) + " ORDER BY srid"; break;
  case Ingres: sql = "SELECT TOP 1 srid FROM spatial_ref_sys WHERE auth_name = 'EPSG' AND auth_srid = " + string_cast<char>(epsg) + " ORDER BY srid"; break;
  case MS_SQL:
  case MySQL: return epsg;
  case Oracle: sql = "SELECT SRID FROM MDSYS.SDO_COORD_REF_SYS WHERE DATA_SOURCE = 'EPSG' AND SRID = " + string_cast<char>(epsg); break;
  case Postgres: sql = "SELECT srid FROM public.spatial_ref_sys WHERE auth_name = 'EPSG' AND auth_srid = " + string_cast<char>(epsg) + " ORDER BY srid FETCH FIRST 1 ROWS ONLY"; break;
  case SQLite: sql = "SELECT srid FROM spatial_ref_sys WHERE auth_name = 'epsg' AND auth_srid = " + string_cast<char>(epsg) + " ORDER BY srid LIMIT 1";
  }

  cmd->exec(sql);
  std::vector<variant> row;
  int srid(-1);
  if (!cmd->fetch(row) || !numeric_cast(row[0], srid)) throw std::runtime_error("SRID error");
  return srid;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_SRID_HPP
