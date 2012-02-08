// Andrew Naplavkov

#ifndef BRIG_DATABASE_SQLITE_DETAIL_LINK_HPP
#define BRIG_DATABASE_SQLITE_DETAIL_LINK_HPP

#include <algorithm>
#include <brig/database/detail/link.hpp>
#include <brig/database/sqlite/detail/binding.hpp>
#include <brig/database/sqlite/detail/db_handle.hpp>
#include <brig/database/sqlite/detail/lib.hpp>
#include <cstring>
#include <memory>
#include <string>

namespace brig { namespace database { namespace sqlite { namespace detail {

class link : public brig::database::detail::link {
  std::shared_ptr<db_handle> m_db;
  sqlite3_stmt* m_stmt;
  bool m_done;
  std::string m_sql;
  void close_stmt();
public:
  link(std::shared_ptr<db_handle> db) : m_db(db), m_stmt(0), m_done(false)  {}
  virtual ~link()  { close_stmt(); }
  virtual DBMS system()  { return SQLite; }
  virtual void exec
    ( const std::string& sql
    , const std::vector<variant>& params = std::vector<variant>()
    , const std::vector<column_detail>& param_cols = std::vector<column_detail>()
    );
  virtual size_t affected()  { return m_db->affected(); }
  virtual void columns(std::vector<std::string>& cols);
  virtual bool fetch(std::vector<variant>& row);
  virtual void start()  { exec("BEGIN"); }
  virtual void commit()  { exec("COMMIT"); }
  virtual void rollback()  { exec("ROLLBACK"); }
}; // link

inline void link::close_stmt()
{
  if (!m_stmt) return;
  m_sql = "";
  m_done = false;
  sqlite3_stmt* stmt(0); std::swap(stmt, m_stmt);
  lib::singleton().p_sqlite3_finalize(stmt);
}

inline void link::exec(const std::string& sql, const std::vector<variant>& params, const std::vector<column_detail>& /*param_cols*/)
{
  if (!m_stmt || !m_done || sql != m_sql)
  {
    close_stmt();
    m_stmt = m_db->prepare_stmt(sql);
    m_sql = sql;
  }
  else
  {
    m_db->check(lib::singleton().p_sqlite3_reset(m_stmt));
    m_done = false;
  }

  for (size_t i(0); i < params.size(); ++i)
    m_db->check(bind(m_stmt, i, params[i]));

  switch (lib::singleton().p_sqlite3_step(m_stmt))
  {
  default: m_db->error(); break;
  case SQLITE_ROW: break;
  case SQLITE_DONE: m_done = true; break;
  }
}

inline void link::columns(std::vector<std::string>& cols)
{
  for (int i(0), count(m_stmt? lib::singleton().p_sqlite3_column_count(m_stmt): 0); i < count; ++i)
    cols.push_back(lib::singleton().p_sqlite3_column_name(m_stmt, i));
}

inline bool link::fetch(std::vector<variant>& row)
{
  if (!m_stmt || m_done) return false;

  const int count(lib::singleton().p_sqlite3_column_count(m_stmt));
  row.resize(count);
  for (int i(0); i < count; ++i)
    switch (lib::singleton().p_sqlite3_column_type(m_stmt, i))
    {
    default: row[i] = null_t(); break;
    case SQLITE_INTEGER: row[i] = int64_t(lib::singleton().p_sqlite3_column_int64(m_stmt, i)); break;
    case SQLITE_FLOAT: row[i] = lib::singleton().p_sqlite3_column_double(m_stmt, i); break;
    case SQLITE_TEXT: row[i] = std::string((const char*)lib::singleton().p_sqlite3_column_text(m_stmt, i)); break;
    case SQLITE_BLOB:
      row[i] = blob_t();
      brig::blob_t& blob = boost::get<brig::blob_t>(row[i]);
      blob.resize(lib::singleton().p_sqlite3_column_bytes(m_stmt, i));
      if (!blob.empty()) memcpy(blob.data(), lib::singleton().p_sqlite3_column_blob(m_stmt, i), blob.size());
      break;
    }

  switch (lib::singleton().p_sqlite3_step(m_stmt))
  {
  default: m_db->error(); return false;
  case SQLITE_ROW: return true;
  case SQLITE_DONE: m_done = true; return true;
  }
} // link::

} } } } // brig::database::sqlite::detail

#endif // BRIG_DATABASE_SQLITE_DETAIL_LINK_HPP
