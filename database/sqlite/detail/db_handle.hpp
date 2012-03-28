// Andrew Naplavkov

#ifndef BRIG_DATABASE_SQLITE_DETAIL_DB_HANDLE_HPP
#define BRIG_DATABASE_SQLITE_DETAIL_DB_HANDLE_HPP

#include <algorithm>
#include <boost/utility.hpp>
#include <brig/database/sqlite/detail/lib.hpp>
#include <stdexcept>
#include <string>

namespace brig { namespace database { namespace sqlite { namespace detail {

class db_handle : ::boost::noncopyable {
  sqlite3* m_db;
public:
  db_handle(const std::string& file);
  virtual ~db_handle()  { lib::singleton().p_sqlite3_close(m_db); }
  size_t affected()  { return lib::singleton().p_sqlite3_changes(m_db); }
  void check(int r)  { if (SQLITE_OK != r) error(); }
  void error();
  sqlite3_stmt* prepare_stmt(const std::string& sql);
}; // db_handle

inline void db_handle::error()
{
  std::string msg;
  if (m_db) msg = lib::singleton().p_sqlite3_errmsg(m_db);
  if (msg.empty()) msg = "SQLite error";
  throw std::runtime_error(msg);
}

inline db_handle::db_handle(const std::string& file) : m_db(0)
{
  if (lib::singleton().empty()) error();
  sqlite3* db(0);
  check(lib::singleton().p_sqlite3_open(file.c_str(), &db));
  std::swap(db, m_db);
}

inline sqlite3_stmt* db_handle::prepare_stmt(const std::string& sql)
{
  sqlite3_stmt* stmt(0);
  check(lib::singleton().p_sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, 0));
  return stmt;
} // db_handle::

} } } } // brig::database::sqlite::detail

#endif // BRIG_DATABASE_SQLITE_DETAIL_DB_HANDLE_HPP
