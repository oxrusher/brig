// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_CREATE_HPP
#define BRIG_DATABASE_DETAIL_SQL_CREATE_HPP

#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <brig/database/column_abstract.hpp>
#include <brig/database/detail/sql_identifier.hpp>
#include <brig/database/global.hpp>
#include <brig/database/table_detail.hpp>
#include <locale>
#include <stdexcept>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

inline void sql_create(DBMS sys, table_detail<column_abstract>& tbl, std::vector<std::string>& sqls)
{
  using namespace ::boost::algorithm;
  auto loc = std::locale::classic();

  tbl.table.schema = "";
  if (Oracle == sys)
    // The TABLE_NAME and COLUMN_NAME values are always converted to uppercase when you insert them into the USER_SDO_GEOM_METADATA view
    to_upper(tbl.table.name, loc);

  std::ostringstream stream; stream.imbue(loc);
  stream << "CREATE TABLE " << sql_identifier(sys, tbl.table.name) << " (";
  bool first(true);
  for (auto p_col = tbl.columns.begin(); p_col != tbl.columns.end(); ++p_col)
  {
    if (Geometry == p_col->type)
      switch (sys)
      {
      default: break;
      case Postgres:
      case SQLite: continue;
      case Oracle:
        // The TABLE_NAME and COLUMN_NAME values are always converted to uppercase when you insert them into the USER_SDO_GEOM_METADATA view
        for (auto p_idx = tbl.indexes.begin(); p_idx != tbl.indexes.end(); ++p_idx)
          for (auto p_col_name = p_idx->columns.begin(); p_col_name != p_idx->columns.end(); ++p_col_name)
            if (*p_col_name == p_col->name)
              to_upper(*p_col_name, loc);
        to_upper(p_col->name, loc);
        break; 
      }

    if (first)  { stream << ", "; first = false; }

    stream << sql_identifier(sys, p_col->name) << " ";

    bool not_null(false);
    switch (sys)
    {
    case VoidSystem: throw std::runtime_error("SQL error");

    case DB2:
      switch (p_col->type)
      {
      case VoidColumn: throw std::runtime_error("SQL error");
      case Date: stream << "DATE"; break;
      case DateTime: stream << "TIMESTAMP"; break;
      case Double: stream << "DOUBLE"; break;
      case Geometry: stream << "DB2GSE.ST_GEOMETRY"; break;
      case Integer: stream << "BIGINT"; break;
      case String: stream << "VARGRAPHIC(255)"; break;
      }
      // When UNIQUE is used, null values are treated as any other values. For example, if the key is a single column that may contain null values, that column may contain no more than one null value.
      for (auto p_idx = tbl.indexes.begin(); p_idx != tbl.indexes.end(); ++p_idx)
        if (Primary == p_idx->type || Unique == p_idx->type)
          for (auto p_col_name = p_idx->columns.begin(); p_col_name != p_idx->columns.end(); ++p_col_name)
            if (*p_col_name == p_col->name)
              not_null = true;
      break;

    case MS_SQL:
      switch (p_col->type)
      {
      case VoidColumn: throw std::runtime_error("SQL error");
      case Date: stream << "DATE"; break;
      case DateTime: stream << "DATETIME"; break;
      case Double: stream << "FLOAT"; break;
      case Geometry: stream << "GEOMETRY"; break;
      case Integer: stream << "BIGINT"; break;
      case String: stream << "NVARCHAR(255)"; break;
      }
      break;

    case MySQL:
      switch (p_col->type)
      {
      case VoidColumn: throw std::runtime_error("SQL error");
      case Date: stream << "DATE"; break;
      case DateTime: stream << "DATETIME"; break;
      case Double: stream << "DOUBLE"; break;
      case Geometry:
        stream << "GEOMETRY";
        // columns in spatial indexes must be declared NOT NULL
        for (auto p_idx = tbl.indexes.begin(); p_idx != tbl.indexes.end(); ++p_idx)
          if (Spatial == p_idx->type)
          {
            if (1 != p_idx->columns.size()) throw std::runtime_error("SQL error");
            if (p_idx->columns.front() == p_col->name) not_null = true;
          }
        break;
      case Integer: stream << "BIGINT"; break;
      case String: stream << "NVARCHAR(255)"; break;
      }
      break;

    case Oracle:
      switch (p_col->type)
      {
      case VoidColumn: throw std::runtime_error("SQL error");
      case Date: stream << "DATE"; break;
      case DateTime: stream << "TIMESTAMP"; break;
      case Double: stream << "BINARY_DOUBLE"; break;
      case Geometry: stream << "MDSYS.SDO_GEOMETRY"; break;
      case Integer: stream << "NUMBER(19)"; break;
      case String: stream << "NVARCHAR2(255)"; break;
      }
      break;

    case Postgres:
      switch (p_col->type)
      {
      case VoidColumn:
      case Geometry: throw std::runtime_error("SQL error");
      case Date: stream << "DATE"; break;
      case DateTime: stream << "TIMESTAMP"; break;
      case Double: stream << "DOUBLE PRECISION"; break;
      case Integer: stream << "BIGINT"; break;
      case String: stream << "VARCHAR(255)"; break;
      }
      break;

    case SQLite:
      switch (p_col->type)
      {
      case VoidColumn:
      case Geometry: throw std::runtime_error("SQL error");
      case Date: stream << "DATE"; break; // numeric affinity
      case DateTime: stream << "DATETIME"; break; // numeric affinity
      case Double: stream << "REAL"; break; // real affinity
      case Integer: stream << "INTEGER"; break; // integer affinity
      case String: stream << "TEXT"; break; // text affinity
      }
      break;
    }

    if (not_null) stream << " NOT NULL";
  }

  // primary key
  auto p_idx = std::find_if(tbl.indexes.begin(), tbl.indexes.end(), [&](const index_detail& idx){ return Primary == idx.type; });
  if (p_idx != tbl.indexes.end())
  {
    p_idx->index = object();
    stream << ", PRIMARY KEY (";
    first = true;
    for (auto p_col_name = p_idx->columns.begin(); p_col_name != p_idx->columns.end(); ++p_col_name)
    {
      if (first)  { stream << ", "; first = false; }
      stream << sql_identifier(sys, *p_col_name);
    }
    stream << ")";
  }
  stream << ")";
  if (MySQL == sys) stream << " ENGINE = MyISAM";
  sqls.push_back(stream.str());

  // geometry
  for (auto p_col = tbl.columns.begin(); p_col != tbl.columns.end(); ++p_col)
    if (Geometry == p_col->type)
    {
      stream = std::ostringstream(); stream.imbue(loc);
      switch (sys)
      {
      case VoidSystem: throw std::runtime_error("SQL error");
      case DB2: stream << "BEGIN ATOMIC DECLARE msg_code INTEGER; DECLARE msg_text VARCHAR(1024); call DB2GSE.ST_register_spatial_column(NULL, '" << sql_identifier(sys, tbl.table.name) << "', '" << sql_identifier(sys, p_col->name) << "', (SELECT SRS_NAME FROM DB2GSE.ST_SPATIAL_REFERENCE_SYSTEMS WHERE ORGANIZATION LIKE 'EPSG' AND ORGANIZATION_COORDSYS_ID = " << p_col->epsg << " ORDER BY SRS_ID FETCH FIRST 1 ROWS ONLY), msg_code, msg_text); END"; break;
      case MS_SQL:
      case MySQL: break;
      case Oracle:
        {
        if (p_col->mbr_need.type() != typeid(brig::boost::box)) throw std::runtime_error("SQL error");
        auto box = ::boost::get<brig::boost::box>(p_col->mbr_need);
        const double xmin(box.min_corner().get<0>()), ymin(box.min_corner().get<1>()), xmax(box.max_corner().get<0>()), ymax(box.max_corner().get<1>()), eps(0.000001);
        stream << "BEGIN DELETE FROM MDSYS.USER_SDO_GEOM_METADATA WHERE TABLE_NAME = '" << tbl.table.name << "' AND COLUMN_NAME = '" << p_col->name << "'; INSERT INTO MDSYS.USER_SDO_GEOM_METADATA (TABLE_NAME, COLUMN_NAME, DIMINFO, SRID) VALUES ('" << tbl.table.name << "', '" << p_col->name << "', MDSYS.SDO_DIM_ARRAY(MDSYS.SDO_DIM_ELEMENT('X', " << xmin << ", " << xmax << ", " << eps << "), MDSYS.SDO_DIM_ELEMENT('Y', " << ymin << ", " << ymax << ", " << eps << ")), (SELECT SRID FROM MDSYS.SDO_COORD_REF_SYS WHERE DATA_SOURCE LIKE 'EPSG' AND SRID = " << p_col->epsg << " AND ROWNUM <= 1)); END;";
        }
        break;
      case Postgres: stream << "SELECT AddGeometryColumn('" << tbl.table.name << "', '" << p_col->name << "', (SELECT SRID FROM PUBLIC.SPATIAL_REF_SYS WHERE AUTH_NAME LIKE 'EPSG' AND AUTH_SRID = " << p_col->epsg << " ORDER BY SRID FETCH FIRST 1 ROWS ONLY), 'GEOMETRY', 2)"; break;
      case SQLite: stream << "SELECT AddGeometryColumn('" << tbl.table.name << "', '" << p_col->name << "', (SELECT SRID FROM SPATIAL_REF_SYS WHERE AUTH_NAME LIKE 'EPSG' AND AUTH_SRID = " << p_col->epsg << " ORDER BY SRID LIMIT 1), 'GEOMETRY', 2)"; break;
      }
      const std::string str(stream.str()); if (!str.empty()) sqls.push_back(str);
    }

  // indexes
  for (auto p_idx = tbl.indexes.begin(); p_idx != tbl.indexes.end(); ++p_idx)
  {
    if (Primary == p_idx->type) continue;

    p_idx->index.schema = "";
    p_idx->index.name = tbl.table.name;
    for (auto p_col_name = p_idx->columns.begin(); p_col_name != p_idx->columns.end(); ++p_col_name)
      p_idx->index.name += "_" + *p_col_name;

    stream = std::ostringstream(); stream.imbue(loc);
    if (Spatial == p_idx->type)
    {
      if (1 != p_idx->columns.size()) throw std::runtime_error("SQL error");
      switch (sys)
      {
      case VoidSystem: throw std::runtime_error("SQL error");
      case DB2: stream << "CREATE INDEX " << sql_identifier(sys, p_idx->index.name) << " ON " << sql_identifier(sys, tbl.table.name) << " (" << sql_identifier(sys, p_idx->columns.front()) << ") EXTEND USING DB2GSE.SPATIAL_INDEX (1, 0, 0)"; break;
      case MS_SQL:
        {
        auto p_col = std::find_if(tbl.columns.begin(), tbl.columns.end(), [&](const column_abstract& col){ return col.name == p_idx->columns.front(); });
        if (p_col == tbl.columns.end() || p_col->mbr_need.type() != typeid(brig::boost::box)) throw std::runtime_error("SQL error");
        auto box = ::boost::get<brig::boost::box>(p_col->mbr_need);
        const double xmin(box.min_corner().get<0>()), ymin(box.min_corner().get<1>()), xmax(box.max_corner().get<0>()), ymax(box.max_corner().get<1>());
        stream << "CREATE SPATIAL INDEX " << sql_identifier(sys, p_idx->index.name) << " ON " << sql_identifier(sys, tbl.table.name) << " (" << sql_identifier(sys, p_idx->columns.front()) << ") USING GEOMETRY_GRID WITH (BOUNDING_BOX = (" << xmin << ", " << ymin << ", " << xmax << ", " << ymax << "))";
        }
        break;
      case MySQL: stream << "CREATE SPATIAL INDEX " << sql_identifier(sys, p_idx->index.name) << " ON " << sql_identifier(sys, tbl.table.name) << " (" << sql_identifier(sys, p_idx->columns.front()) << ")"; break;
      case Oracle: stream << "CREATE INDEX " << sql_identifier(sys, p_idx->index.name) << " ON " << sql_identifier(sys, tbl.table.name) << " (" << sql_identifier(sys, p_idx->columns.front()) << ") INDEXTYPE IS MDSYS.SPATIAL_INDEX"; break;
      case Postgres: stream << "CREATE INDEX " << sql_identifier(sys, p_idx->index.name) << " ON " << sql_identifier(sys, tbl.table.name) << " USING GIST(" << sql_identifier(sys, p_idx->columns.front()) << ")"; break;
      case SQLite: stream << "SELECT CreateSpatialIndex('" << tbl.table.name << "', '" << p_idx->columns.front() << "')"; p_idx->index.name = ""; break;
      }
    }
    else
    {
      stream << "CREATE ";
      if (Unique == p_idx->type) stream << "UNIQUE ";
      stream << "INDEX " << sql_identifier(sys, p_idx->index.name) << " ON " << sql_identifier(sys, tbl.table.name) << " (";
      first = true;
      for (auto p_col_name = p_idx->columns.begin(); p_col_name != p_idx->columns.end(); ++p_col_name)
      {
        if (first)  { stream << ", "; first = false; }
        stream << sql_identifier(sys, *p_col_name);
      }
      stream << ")";
    }
    sqls.push_back(stream.str());
  }
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_CREATE_HPP
