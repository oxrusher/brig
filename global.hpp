// Andrew Naplavkov

#ifndef BRIG_GLOBAL_HPP
#define BRIG_GLOBAL_HPP

namespace brig {

const char LibVersion[] = "1306";

const int CharsLimit = 250;
const size_t PageSize = 250; // DB2 PUERTO_ROADS is slowdown after 447
const size_t PoolSize = 4;
const size_t TimeoutSec = 120;

const char TableName[] = "tbl";
const char ColumnNameWkb[] = "wkb";
const char ColumnNamePng[] = "png";

const char LibCubridWin[] = "cascci.dll";
const char LibCubridLin[] = "libcascci.so";
const char LibCurlWin[] = "libcurl.dll";
const char LibCurlLin[] = "libcurl.so";
const char LibGdalWin[] = "gdal110.dll";
const char LibGdalLin[] = "libgdal.so";
const char LibMysqlWin[] = "libmysql.dll";
const char LibMysqlLin[] = "libmysqlclient.so";
const char LibOdbcWin[] = "odbc32.dll";
const char LibOdbcLin[] = "libodbc.so";
const char LibPostgresWin[] = "libpq.dll";
const char LibPostgresLin[] = "libpq.so";
const char LibProjWin[] = "proj.dll";
const char LibProjLin[] = "libproj.so.0";
const char LibSpatialiteWin[] = "spatialite.dll";
const char LibSpatialiteLin[] = "libspatialite.so";
const char LibSqliteWin[] = "sqlite3.dll";
const char LibSqliteLin[] = "libsqlite3.so.0";

} // brig

#endif // BRIG_GLOBAL_HPP
