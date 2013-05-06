// Andrew Naplavkov

#ifndef BRIG_DATABASE_DBMS_HPP
#define BRIG_DATABASE_DBMS_HPP

namespace brig { namespace database {

enum class DBMS {
  Void,
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

} } // brig::database

#endif // BRIG_DATABASE_DBMS_HPP
