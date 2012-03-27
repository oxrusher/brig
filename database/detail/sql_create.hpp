// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_CREATE_HPP
#define BRIG_DATABASE_DETAIL_SQL_CREATE_HPP

#include <algorithm>
#include <brig/database/column_definition.hpp>
#include <brig/database/detail/normalize_identifier.hpp>
#include <brig/database/detail/sql_identifier.hpp>
#include <brig/database/global.hpp>
#include <brig/database/identifier.hpp>
#include <brig/database/index_definition.hpp>
#include <brig/database/table_definition.hpp>
#include <brig/string_cast.hpp>
#include <locale>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

inline void sql_create(DBMS sys, table_definition tbl, std::vector<std::string>& sql)
{
  static const int CharsLimit = 250;

  if (VoidSystem == sys) throw std::runtime_error("SQL error");
  auto col_end( std::remove_if(std::begin(tbl.columns), std::end(tbl.columns), [](const column_definition& c){ return VoidColumn == c.type; }) );
  for (auto idx(std::begin(tbl.indexes)); idx != std::end(tbl.indexes); ++idx)
    for (auto col_name(std::begin(idx->columns)); col_name != std::end(idx->columns); ++col_name)
      if (std::find_if(std::begin(tbl.columns), col_end, [&](const column_definition& c){ return c.name == *col_name; }) == col_end)
        idx->type = VoidIndex;
  auto idx_end( std::remove_if(std::begin(tbl.indexes), std::end(tbl.indexes), [](const index_definition& i){ return VoidIndex == i.type; }) );
  normalize_identifier(sys, tbl.id);

  // columns
  auto loc = std::locale::classic();
  std::ostringstream stream; stream.imbue(loc);
  stream << "CREATE TABLE " << sql_identifier(sys, tbl.id.name) << " (";
  bool first(true);
  for (auto col(std::begin(tbl.columns)); col != col_end; ++col)
  {
    if (Geometry == col->type)
      switch (sys)
      {
      case Postgres:
      case SQLite: continue;
      case Oracle:
        // The TABLE_NAME and COLUMN_NAME values are always converted to uppercase when you insert them into the USER_SDO_GEOM_METADATA view
        for (auto idx(std::begin(tbl.indexes)); idx != idx_end; ++idx)
          for (auto col_name(std::begin(idx->columns)); col_name != std::end(idx->columns); ++col_name)
            if (*col_name == col->name)
              normalize_identifier(sys, *col_name);
        normalize_identifier(sys, col->name);
        break; 
      }

    if (first) first = false;
    else stream << ", ";
    stream << sql_identifier(sys, col->name) << " ";
    int chars(CharsLimit);
    if (col->chars > 0 && col->chars < CharsLimit) chars = col->chars;

    switch (sys)
    {
    case DB2:
      switch (col->type)
      {
      case Blob: stream << "BLOB"; break;
      case Double: stream << "DOUBLE"; break;
      case Geometry: stream << "DB2GSE.ST_GEOMETRY"; break;
      case Integer: stream << "BIGINT"; break;
      case String: stream << "VARGRAPHIC(" << chars << ")"; break;
      }
      // When UNIQUE is used, null values are treated as any other values. For example, if the key is a single column that may contain null values, that column may contain no more than one null value.
      for (auto idx(std::begin(tbl.indexes)); idx != idx_end; ++idx)
        if (Primary == idx->type || Unique == idx->type)
          for (auto col_name(std::begin(idx->columns)); col_name != std::end(idx->columns); ++col_name)
            if (*col_name == col->name)
              col->not_null = true;
      break;

    case MS_SQL:
      switch (col->type)
      {
      case Blob: stream << "VARBINARY(MAX)"; break;
      case Double: stream << "FLOAT"; break;
      case Geometry: stream << "GEOMETRY"; break;
      case Integer: stream << "BIGINT"; break;
      case String: stream << "NVARCHAR(" << chars << ")"; break;
      }
      break;

    case MySQL:
      switch (col->type)
      {
      case Blob: stream << "LONGBLOB"; break;
      case Double: stream << "DOUBLE"; break;
      case Geometry:
        stream << "GEOMETRY";
        // columns in spatial indexes must be declared NOT NULL
        for (auto idx(std::begin(tbl.indexes)); idx != idx_end; ++idx)
          if (Spatial == idx->type)
          {
            if (1 != idx->columns.size()) throw std::runtime_error("table error");
            if (idx->columns.front() == col->name) col->not_null = true;
          }
        break;
      case Integer: stream << "BIGINT"; break;
      case String: stream << "NVARCHAR(" << chars << ")"; break;
      }
      break;

    case Oracle:
      switch (col->type)
      {
      case Blob: stream << "BLOB"; break;
      case Double: stream << "BINARY_DOUBLE"; break;
      case Geometry: stream << "MDSYS.SDO_GEOMETRY"; break;
      case Integer: stream << "NUMBER(19)"; break;
      case String: stream << "NVARCHAR2(" << chars << ")"; break;
      }
      break;

    case Postgres:
      switch (col->type)
      {
      case Blob: stream << "BYTEA"; break;
      case Double: stream << "DOUBLE PRECISION"; break;
      case Integer: stream << "BIGINT"; break;
      case String: stream << "VARCHAR(" << chars << ")"; break;
      }
      break;

    case SQLite:
      switch (col->type)
      {
      case Blob: stream << "BLOB"; break;
      case Double: stream << "REAL"; break; // real affinity
      case Integer: stream << "INTEGER"; break; // integer affinity
      case String: stream << "TEXT"; break; // text affinity
      }
      break;
    }

    if (col->not_null) stream << " NOT NULL";
  }

  // primary key
  auto idx(std::find_if(std::begin(tbl.indexes), idx_end, [&](const index_definition& i){ return Primary == i.type; }));
  if (idx != idx_end)
  {
    idx->id = identifier();
    stream << ", PRIMARY KEY (";
    first = true;
    for (auto col_name(std::begin(idx->columns)); col_name != std::end(idx->columns); ++col_name)
    {
      if (first) first = false;
      else stream << ", ";
      stream << sql_identifier(sys, *col_name);
    }
    stream << ")";
  }
  stream << ")";
  if (MySQL == sys) stream << " ENGINE = MyISAM";
  sql.push_back(stream.str());

  // geometry
  for (auto col(std::begin(tbl.columns)); col != col_end; ++col)
    if (Geometry == col->type)
    {
      stream = std::ostringstream(); stream.imbue(loc);
      switch (sys)
      {
      case DB2: stream << "BEGIN ATOMIC DECLARE msg_code INTEGER; DECLARE msg_text VARCHAR(1024); call DB2GSE.ST_register_spatial_column(NULL, '" << sql_identifier(sys, tbl.id.name) << "', '" << sql_identifier(sys, col->name) << "', (SELECT SRS_NAME FROM DB2GSE.ST_SPATIAL_REFERENCE_SYSTEMS WHERE ORGANIZATION LIKE 'EPSG' AND ORGANIZATION_COORDSYS_ID = " << col->epsg << " ORDER BY SRS_ID FETCH FIRST 1 ROWS ONLY), msg_code, msg_text); END"; break;
      case MS_SQL:
      case MySQL: break;
      case Oracle:
        {
        if (col->mbr.type() != typeid(brig::boost::box)) throw std::runtime_error("SQL error");
        auto box(::boost::get<brig::boost::box>(col->mbr));
        const double xmin(box.min_corner().get<0>()), ymin(box.min_corner().get<1>()), xmax(box.max_corner().get<0>()), ymax(box.max_corner().get<1>()), eps(0.000001);
        stream << "BEGIN DELETE FROM MDSYS.USER_SDO_GEOM_METADATA WHERE TABLE_NAME = '" << tbl.id.name << "' AND COLUMN_NAME = '" << col->name << "'; INSERT INTO MDSYS.USER_SDO_GEOM_METADATA (TABLE_NAME, COLUMN_NAME, DIMINFO, SRID) VALUES ('" << tbl.id.name << "', '" << col->name << "', MDSYS.SDO_DIM_ARRAY(MDSYS.SDO_DIM_ELEMENT('X', " << xmin << ", " << xmax << ", " << eps << "), MDSYS.SDO_DIM_ELEMENT('Y', " << ymin << ", " << ymax << ", " << eps << ")), (SELECT SRID FROM MDSYS.SDO_COORD_REF_SYS WHERE DATA_SOURCE LIKE 'EPSG' AND SRID = " << col->epsg << " AND ROWNUM <= 1)); END;";
        }
        break;
      case Postgres: stream << "SELECT AddGeometryColumn('" << tbl.id.name << "', '" << col->name << "', (SELECT SRID FROM PUBLIC.SPATIAL_REF_SYS WHERE AUTH_NAME LIKE 'EPSG' AND AUTH_SRID = " << col->epsg << " ORDER BY SRID FETCH FIRST 1 ROWS ONLY), 'GEOMETRY', 2)"; break;
      case SQLite: stream << "SELECT AddGeometryColumn('" << tbl.id.name << "', '" << col->name << "', (SELECT SRID FROM SPATIAL_REF_SYS WHERE AUTH_NAME LIKE 'EPSG' AND AUTH_SRID = " << col->epsg << " ORDER BY SRID LIMIT 1), 'GEOMETRY', 2)"; break;
      }
      const std::string str(stream.str()); if (!str.empty()) sql.push_back(str);
    }

  // indexes
  size_t counter(0);
  for (auto idx(std::begin(tbl.indexes)); idx != idx_end; ++idx)
  {
    if (Primary == idx->type) continue;
    if (idx->columns.empty()) throw std::runtime_error("table error");
    idx->id.name = tbl.id.name + "_idx_" + string_cast<char>(++counter);

    stream = std::ostringstream(); stream.imbue(loc);
    if (Spatial == idx->type)
    {
      if (1 != idx->columns.size()) throw std::runtime_error("table error");
      switch (sys)
      {
      case DB2: stream << "CREATE INDEX " << sql_identifier(sys, idx->id.name) << " ON " << sql_identifier(sys, tbl.id.name) << " (" << sql_identifier(sys, idx->columns.front()) << ") EXTEND USING DB2GSE.SPATIAL_INDEX (1, 0, 0)"; break;
      case MS_SQL:
        {
        auto col(std::find_if(std::begin(tbl.columns), col_end, [&](const column_definition& c){ return c.name == idx->columns.front(); }));
        if (col == col_end || col->mbr.type() != typeid(brig::boost::box)) throw std::runtime_error("SQL error");
        auto box(::boost::get<brig::boost::box>(col->mbr));
        const double xmin(box.min_corner().get<0>()), ymin(box.min_corner().get<1>()), xmax(box.max_corner().get<0>()), ymax(box.max_corner().get<1>());
        stream << "CREATE SPATIAL INDEX " << sql_identifier(sys, idx->id.name) << " ON " << sql_identifier(sys, tbl.id.name) << " (" << sql_identifier(sys, idx->columns.front()) << ") USING GEOMETRY_GRID WITH (BOUNDING_BOX = (" << xmin << ", " << ymin << ", " << xmax << ", " << ymax << "))";
        }
        break;
      case MySQL: stream << "CREATE SPATIAL INDEX " << sql_identifier(sys, idx->id.name) << " ON " << sql_identifier(sys, tbl.id.name) << " (" << sql_identifier(sys, idx->columns.front()) << ")"; break;
      case Oracle: stream << "CREATE INDEX " << sql_identifier(sys, idx->id.name) << " ON " << sql_identifier(sys, tbl.id.name) << " (" << sql_identifier(sys, idx->columns.front()) << ") INDEXTYPE IS MDSYS.SPATIAL_INDEX"; break;
      case Postgres: stream << "CREATE INDEX " << sql_identifier(sys, idx->id.name) << " ON " << sql_identifier(sys, tbl.id.name) << " USING GIST(" << sql_identifier(sys, idx->columns.front()) << ")"; break;
      case SQLite: stream << "SELECT CreateSpatialIndex('" << tbl.id.name << "', '" << idx->columns.front() << "')"; idx->id.name = ""; break;
      }
    }
    else
    {
      stream << "CREATE ";
      if (Unique == idx->type) stream << "UNIQUE ";
      stream << "INDEX " << sql_identifier(sys, idx->id.name) << " ON " << sql_identifier(sys, tbl.id.name) << " (";
      first = true;
      for (auto col_name(std::begin(idx->columns)); col_name != std::end(idx->columns); ++col_name)
      {
        if (first) first = false;
        else stream << ", ";
        stream << sql_identifier(sys, *col_name);
      }
      stream << ")";
    }
    sql.push_back(stream.str());
  }
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_CREATE_HPP
