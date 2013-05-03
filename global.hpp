// Andrew Naplavkov

#ifndef BRIG_GLOBAL_HPP
#define BRIG_GLOBAL_HPP

namespace brig {

enum column_type {
  VoidColumn,
  Blob,
  Double,
  Geometry,
  Integer,
  String
}; // column_type

enum index_type {
  VoidIndex,
  Primary,
  Unique,
  Duplicate,
  Spatial
}; // index_type

const int CharsLimit = 250;
const size_t PageSize = 250; // DB2 PUERTO_ROADS is slowdown after 447
const size_t PoolSize = 8;
const size_t TimeoutSec = 120;

const char TBL[] = "tbl";
const char WKB[] = "wkb";
const char PNG[] = "png";

const char LibCubridWin[] = "cascci.dll";
const char LibCubridLin[] = "libcascci.so";
const char LibCurlWin[] = "libcurl.dll";
const char LibCurlLin[] = "libcurl.so";
const char LibGdalWin[] = "gdal110.dll"; // "libgdal-1.dll"
const char LibGdalLin[] = "libgdal.so";
const char LibMysqlWin[] = "libmysql.dll";
const char LibMysqlLin[] = "libmysqlclient.so";
const char LibOdbcWin[] = "odbc32.dll";
const char LibOdbcLin[] = "libodbc.so";
const char LibPostgresWin[] = "libpq.dll";
const char LibPostgresLin[] = "libpq.so";
const char LibProjWin[] = "proj.dll"; // "libproj-0.dll"
const char LibProjLin[] = "libproj.so.0";
const char LibSpatialiteWin[] = "spatialite.dll"; // "libspatialite.dll"
const char LibSpatialiteLin[] = "libspatialite.so";
const char LibSqliteWin[] = "sqlite3.dll"; // "libsqlite3-0.dll"
const char LibSqliteLin[] = "libsqlite3.so.0";

} // brig

#endif // BRIG_GLOBAL_HPP
