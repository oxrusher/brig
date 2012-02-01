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

enum Type {
  UnknownType,
  Date,
  DateTime,
  Double,
  Geometry,
  Integer,
  String
}; // Type

static const size_t PageSize = 250;

} } // brig::database

#endif // BRIG_DATABASE_GLOBAL_HPP
