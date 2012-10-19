// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_CREATE_HPP
#define BRIG_DATABASE_DETAIL_SQL_CREATE_HPP

#include <algorithm>
#include <brig/boost/envelope.hpp>
#include <brig/boost/geom_from_wkb.hpp>
#include <brig/database/detail/sql_identifier.hpp>
#include <brig/database/global.hpp>
#include <brig/database/table_definition.hpp>
#include <brig/string_cast.hpp>
#include <iterator>
#include <ios>
#include <locale>
#include <stdexcept>
#include <sstream>

namespace brig { namespace database { namespace detail {

inline void sql_create(DBMS sys, const table_definition tbl, std::vector<std::string>& sql)
{
  using namespace std;
  using namespace brig::boost;
  auto loc = locale::classic();

  // columns, primary key
  {
  ostringstream stream; stream.imbue(loc);
  stream << "CREATE TABLE " << sql_identifier(sys, tbl.id.name) << " (";

  bool first(true);
  for (auto col(begin(tbl.columns)); col != end(tbl.columns); ++col)
  {
    if (Geometry == col->type)
      switch (sys)
      {
      default: break;
      case Postgres:
      case SQLite: continue;
      }

    if (first) first = false;
    else stream << ", ";

    stream << sql_identifier(sys, col->name) << " ";
    if (!col->dbms_type.schema.empty()) stream << col->dbms_type.schema << ".";
    stream << col->dbms_type.name;

    if (Geometry == col->type && Ingres == sys) stream << " SRID " << col->srid;
  }

  auto idx(find_if(begin(tbl.indexes), end(tbl.indexes), [&](const index_definition& idx_){ return Primary == idx_.type; }));
  if (idx != end(tbl.indexes))
  {
    stream << ", PRIMARY KEY (";
    bool first(true);
    for (auto idx_col(begin(idx->columns)); idx_col != end(idx->columns); ++idx_col)
    {
      if (first) first = false;
      else stream << ", ";
      stream << sql_identifier(sys, *idx_col);
    }
    stream << ")";
  }

  stream << ")";
  if (MySQL == sys) stream << " ENGINE = MyISAM";
  sql.push_back(stream.str());
  }

  // geometry
  for (auto col(begin(tbl.columns)); col != end(tbl.columns); ++col)
    if (Geometry == col->type)
      switch (sys)
      {
      case VoidSystem:
      case CUBRID:
        throw runtime_error("DBMS error");
      case DB2:
        sql.push_back("BEGIN ATOMIC DECLARE msg_code INTEGER; DECLARE msg_text VARCHAR(1024); call DB2GSE.ST_register_spatial_column(NULL, '" + sql_identifier(sys, tbl.id.name) + "', '" + sql_identifier(sys, col->name) + "', (SELECT SRS_NAME FROM DB2GSE.ST_SPATIAL_REFERENCE_SYSTEMS WHERE SRS_ID = " + string_cast<char>(col->srid) + "), msg_code, msg_text); END");
        break;
      case Informix:
        {
        const std::string catalog("(SELECT RTRIM(ODB_DBName) DB FROM sysmaster:SysOpenDB WHERE CAST(ODB_SessionID AS INT) = CAST(DBINFO('sessionid') AS INT) AND ODB_IsCurrent = 'Y')");
        const std::string user("RTRIM(USER)");
        sql.push_back("DELETE FROM sde.geometry_columns WHERE f_table_catalog = " + catalog + " AND f_table_schema = " + user + " AND f_table_name = 'Mexico' AND f_geometry_column = 'Geometry'");
        sql.push_back("INSERT INTO sde.geometry_columns (f_table_catalog, f_table_schema, f_table_name, f_geometry_column, geometry_type, coord_dimension, srid) VALUES (" + catalog + ", " + user + ", '" + tbl.id.name + "', '" + col->name + "', 0, 2, " + string_cast<char>(col->srid) + ")");
        }
        break;
      case Ingres:
      case MS_SQL:
      case MySQL: break;
      case Oracle:
        {
        sql.push_back("DELETE FROM MDSYS.USER_SDO_GEOM_METADATA WHERE TABLE_NAME = '" + tbl.id.name + "' AND COLUMN_NAME = '" + col->name + "'");
        auto box(envelope(geom_from_wkb(::boost::get<blob_t>(col->query_value))));
        const double xmin(box.min_corner().get<0>()), ymin(box.min_corner().get<1>()), xmax(box.max_corner().get<0>()), ymax(box.max_corner().get<1>());
        ostringstream stream; stream.imbue(loc); stream << scientific; stream.precision(17);
        stream << "INSERT INTO MDSYS.USER_SDO_GEOM_METADATA (TABLE_NAME, COLUMN_NAME, DIMINFO, SRID) VALUES ('" << tbl.id.name << "', '" << col->name << "', MDSYS.SDO_DIM_ARRAY(MDSYS.SDO_DIM_ELEMENT('X', " << xmin << ", " << xmax << ", 0.000001), MDSYS.SDO_DIM_ELEMENT('Y', " << ymin << ", " << ymax << ", 0.000001)), " << col->srid << ")";
        sql.push_back(stream.str());
        }
        break;
      case Postgres:
      case SQLite:
        sql.push_back("SELECT AddGeometryColumn('" + tbl.id.name + "', '" + col->name + "', " + string_cast<char>(col->srid) + ", 'GEOMETRY', 2)");
        break;
      }

  // indexes
  for (auto idx(begin(tbl.indexes)); idx != end(tbl.indexes); ++idx)
  {
    ostringstream stream; stream.imbue(loc); stream << scientific; stream.precision(17);

    if (Spatial == idx->type)
    {
      switch (sys)
      {
      case VoidSystem:
      case CUBRID: throw runtime_error("DBMS error");
      case DB2: stream << "CREATE INDEX " << sql_identifier(sys, idx->id.name) << " ON " << sql_identifier(sys, tbl.id.name) << " (" << sql_identifier(sys, idx->columns.front()) << ") EXTEND USING DB2GSE.SPATIAL_INDEX (1, 0, 0)"; break;
      case Informix: stream << "CREATE INDEX " << sql_identifier(sys, idx->id.name) << " ON " << sql_identifier(sys, tbl.id.name) << " (" << sql_identifier(sys, idx->columns.front()) << " ST_Geometry_Ops) USING RTREE"; break;
      case Ingres:
        {
        auto col(tbl[idx->columns.front()]);
        auto box(envelope(geom_from_wkb(::boost::get<blob_t>(col->query_value))));
        const double xmin(box.min_corner().get<0>()), ymin(box.min_corner().get<1>()), xmax(box.max_corner().get<0>()), ymax(box.max_corner().get<1>());
        stream << "CREATE INDEX " << sql_identifier(sys, idx->id.name) << " ON " << sql_identifier(sys, tbl.id.name) << " (" << sql_identifier(sys, idx->columns.front()) << ") WITH STRUCTURE=RTREE, RANGE=((" << xmin << ", " << ymin << "), (" << xmax << ", " << ymax << "))";
        }
        break;
      case MS_SQL:
        {
        auto col(tbl[idx->columns.front()]);
        auto box(envelope(geom_from_wkb(::boost::get<blob_t>(col->query_value))));
        const double xmin(box.min_corner().get<0>()), ymin(box.min_corner().get<1>()), xmax(box.max_corner().get<0>()), ymax(box.max_corner().get<1>());
        stream << "CREATE SPATIAL INDEX " << sql_identifier(sys, idx->id.name) << " ON " << sql_identifier(sys, tbl.id.name) << " (" << sql_identifier(sys, idx->columns.front()) << ") USING GEOMETRY_GRID WITH (BOUNDING_BOX = (" << xmin << ", " << ymin << ", " << xmax << ", " << ymax << "))";
        }
        break;
      case MySQL: stream << "CREATE SPATIAL INDEX " << sql_identifier(sys, idx->id.name) << " ON " << sql_identifier(sys, tbl.id.name) << " (" << sql_identifier(sys, idx->columns.front()) << ")"; break;
      case Oracle: stream << "CREATE INDEX " << sql_identifier(sys, idx->id.name) << " ON " << sql_identifier(sys, tbl.id.name) << " (" << sql_identifier(sys, idx->columns.front()) << ") INDEXTYPE IS MDSYS.SPATIAL_INDEX"; break;
      case Postgres: stream << "CREATE INDEX " << sql_identifier(sys, idx->id.name) << " ON " << sql_identifier(sys, tbl.id.name) << " USING GIST(" << sql_identifier(sys, idx->columns.front()) << ")"; break;
      case SQLite: stream << "SELECT CreateSpatialIndex('" << tbl.id.name << "', '" << idx->columns.front() << "')"; break;
      }
    }
    else if (Primary != idx->type)
    {
      stream << "CREATE ";
      if (Unique == idx->type) stream << "UNIQUE ";
      stream << "INDEX " << sql_identifier(sys, idx->id.name) << " ON " << sql_identifier(sys, tbl.id.name) << " (";
      bool first(true);
      for (auto idx_col(begin(idx->columns)); idx_col != end(idx->columns); ++idx_col)
      {
        if (first) first = false;
        else stream << ", ";
        stream << sql_identifier(sys, *idx_col);
      }
      stream << ")";
    }

    const string str(stream.str());
    if (!str.empty()) sql.push_back(stream.str());
  }
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_CREATE_HPP
