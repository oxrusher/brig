// Andrew Naplavkov

#ifndef BRIG_DATABASE_GLOBAL_HPP
#define BRIG_DATABASE_GLOBAL_HPP

namespace brig { namespace database {

enum DBMS {
  VoidSystem,
  CUBRID,
  DB2,
  Informix,
  Ingres,
  MS_SQL,
  MySQL,
  Oracle,
  Postgres,
  SQLite
}; // DBMS

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
const size_t PoolSize = 5;

} } // brig::database

#endif // BRIG_DATABASE_GLOBAL_HPP
