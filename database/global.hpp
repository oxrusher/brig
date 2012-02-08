// Andrew Naplavkov

#ifndef BRIG_DATABASE_GLOBAL_HPP
#define BRIG_DATABASE_GLOBAL_HPP

namespace brig { namespace database {

enum DBMS {
  UnknownSystem,
  DB2,
  MS_SQL,
  MySQL,
  Oracle,
  Postgres,
  SQLite
}; // DBMS

enum column_type {
  UnknownType,
  Date,
  DateTime,
  Double,
  Geometry,
  Integer,
  String
}; // column_type

static const size_t PageSize = 250; // DB2 PUERTO_ROADS is slowdown after 447
static const size_t PoolSize = 5;

} } // brig::database

#endif // BRIG_DATABASE_GLOBAL_HPP
