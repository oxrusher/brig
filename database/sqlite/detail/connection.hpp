// Andrew Naplavkov

#ifndef BRIG_DATABASE_SQLITE_DETAIL_CONNECTION_HPP
#define BRIG_DATABASE_SQLITE_DETAIL_CONNECTION_HPP

#include <algorithm>
#include <boost/utility.hpp>
#include <brig/database/sqlite/detail/lib.hpp>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace brig { namespace database { namespace sqlite { namespace detail {

class connection : boost::noncopyable {
  sqlite3* m_dbc;
public:
  connection(const std::string& file);
  virtual ~connection()  { lib::singleton().p_sqlite3_close(m_dbc); }
  int64_t affected()  { return lib::singleton().p_sqlite3_changes(m_dbc); }
  void check(int r)  { if (SQLITE_OK != r) error(); }
  void error();
  sqlite3_stmt* prepare_stmt(const std::string& sql);
  void restore(connection& from);
}; // connection

inline void connection::error()
{
  std::string msg;
  if (m_dbc) msg = lib::singleton().p_sqlite3_errmsg(m_dbc);
  if (msg.empty()) msg = "SQLite error";
  throw std::runtime_error(msg);
}

inline connection::connection(const std::string& file) : m_dbc(0)
{
  if (lib::singleton().empty()) error();
  sqlite3* dbc(0);
  check(lib::singleton().p_sqlite3_open(file.c_str(), &dbc));
  std::swap(dbc, m_dbc);
}

inline sqlite3_stmt* connection::prepare_stmt(const std::string& sql)
{
  sqlite3_stmt* stmt(0);
  check(lib::singleton().p_sqlite3_prepare_v2(m_dbc, sql.c_str(), -1, &stmt, 0));
  return stmt;
}

inline void connection::restore(connection& from)
{
  sqlite3_backup* backup(lib::singleton().p_sqlite3_backup_init(m_dbc, "main", from.m_dbc, "main"));
  if (!backup) error();
  lib::singleton().p_sqlite3_backup_step(backup, -1);
  const int r(lib::singleton().p_sqlite3_backup_finish(backup));
  if (SQLITE_DONE != r && SQLITE_OK != r) error();
} // connection::

} } } } // brig::database::sqlite::detail

#endif // BRIG_DATABASE_SQLITE_DETAIL_CONNECTION_HPP
