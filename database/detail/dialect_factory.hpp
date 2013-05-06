// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_DIALECT_FACTORY_HPP
#define BRIG_DATABASE_DETAIL_DIALECT_FACTORY_HPP

#include <brig/database/detail/dialect.hpp>
#include <brig/database/detail/dialect_cubrid.hpp>
#include <brig/database/detail/dialect_db2.hpp>
#include <brig/database/detail/dialect_informix.hpp>
#include <brig/database/detail/dialect_ingres.hpp>
#include <brig/database/detail/dialect_ms_sql.hpp>
#include <brig/database/detail/dialect_mysql.hpp>
#include <brig/database/detail/dialect_oracle.hpp>
#include <brig/database/detail/dialect_postgres.hpp>
#include <brig/database/detail/dialect_sqlite.hpp>
#include <brig/global.hpp>
#include <stdexcept>

namespace brig { namespace database { namespace detail {

inline dialect* dialect_factory(DBMS sys)
{
  switch (sys)
  {
  case DBMS::Void: break;
  case DBMS::CUBRID: return new dialect_cubrid();
  case DBMS::DB2: return new dialect_db2();
  case DBMS::Informix: return new dialect_informix();
  case DBMS::Ingres: return new dialect_ingres();
  case DBMS::MS_SQL: return new dialect_ms_sql();
  case DBMS::MySQL: return new dialect_mysql();
  case DBMS::Oracle: return new dialect_oracle();
  case DBMS::Postgres: return new dialect_postgres();
  case DBMS::SQLite: return new dialect_sqlite();
  }
  throw std::runtime_error("DBMS error");
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_DIALECT_FACTORY_HPP
