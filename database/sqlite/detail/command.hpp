// Andrew Naplavkov

#ifndef BRIG_DATABASE_SQLITE_DETAIL_COMMAND_HPP
#define BRIG_DATABASE_SQLITE_DETAIL_COMMAND_HPP

#include <algorithm>
#include <brig/database/command.hpp>
#include <brig/database/detail/is_ogc_type.hpp>
#include <brig/database/sqlite/detail/binding.hpp>
#include <brig/database/sqlite/detail/column_geometry.hpp>
#include <brig/database/sqlite/detail/db_handle.hpp>
#include <brig/database/sqlite/detail/lib.hpp>
#include <brig/unicode/lower_case.hpp>
#include <brig/unicode/transform.hpp>
#include <cstring>
#include <exception>
#include <memory>
#include <string>
#include <vector>

namespace brig { namespace database { namespace sqlite { namespace detail {

class command : public brig::database::command
{
  struct column  { std::string name; bool geometry; };

  db_handle m_db;
  sqlite3_stmt* m_stmt;
  std::string m_sql;
  std::vector<column> m_cols;
  bool m_done, m_autocommit;

  void close_stmt();

public:
  explicit command(const std::string& file) : m_db(file), m_stmt(0), m_done(false), m_autocommit(true)  {}
  ~command() override;
  void exec(const std::string& sql, const std::vector<column_def>& params = std::vector<column_def>()) override;
  void exec_batch(const std::string& sql) override;
  std::vector<std::string> columns() override;
  bool fetch(std::vector<variant>& row) override;
  void set_autocommit(bool autocommit) override;
  void commit() override;
  DBMS system() override  { return DBMS::SQLite; }
  bool readable_geom() override { return true; }
}; // command

inline void command::close_stmt()
{
  if (!m_stmt) return;
  m_done = false;
  m_cols.clear();
  m_sql = "";
  sqlite3_stmt* stmt(0); std::swap(stmt, m_stmt);
  lib::singleton().p_sqlite3_finalize(stmt);
}

inline command::~command()
{
  try  { set_autocommit(true); }
  catch (const std::exception&)  {}
  close_stmt();
}

inline void command::exec(const std::string& sql, const std::vector<column_def>& params)
{
  if (!m_stmt || sql.empty() || sql != m_sql || !m_done)
  {
    close_stmt();
    m_stmt = m_db.prepare_stmt(sql);
    m_sql = sql;
  }
  else
  {
    m_db.check(lib::singleton().p_sqlite3_reset(m_stmt));
    m_cols.clear();
    m_done = false;
  }

  for (size_t i(0); i < params.size(); ++i)
    m_db.check(bind(m_stmt, i, params[i].query_value));

  switch (lib::singleton().p_sqlite3_step(m_stmt))
  {
  default: m_db.error(); break;
  case SQLITE_ROW: break;
  case SQLITE_DONE: m_done = true; break;
  }
}

inline void command::exec_batch(const std::string& sql)
{
  close_stmt();
  m_db.exec(sql.c_str());
}

inline std::vector<std::string> command::columns()
{
  std::vector<std::string> cols;
  if (!m_stmt) return cols;
  m_sql = "";
  m_cols.clear();

  for (int i(0), count(lib::singleton().p_sqlite3_column_count(m_stmt)); i < count; ++i)
  {
    auto name_ptr = lib::singleton().p_sqlite3_column_name(m_stmt, i);
    auto type_ptr = lib::singleton().p_sqlite3_column_decltype(m_stmt, i);

    column col;
    if (name_ptr) col.name = name_ptr;
    col.geometry = (type_ptr && brig::database::detail::is_ogc_type(brig::unicode::transform<char>(type_ptr, brig::unicode::lower_case)));

    m_cols.push_back(col);
    cols.push_back(col.name);
  }
  return cols;
}

inline bool command::fetch(std::vector<variant>& row)
{
  if (!m_stmt || m_done) return false;
  if (m_cols.empty()) columns();

  const int count = int(m_cols.size());
  row.resize(count);
  for (int i(0); i < count; ++i)
    switch (lib::singleton().p_sqlite3_column_type(m_stmt, i))
    {
    default: row[i] = null_t(); break;
    case SQLITE_INTEGER: row[i] = int64_t(lib::singleton().p_sqlite3_column_int64(m_stmt, i)); break;
    case SQLITE_FLOAT: row[i] = lib::singleton().p_sqlite3_column_double(m_stmt, i); break;

    case SQLITE_TEXT:
      {
      const char* text_ptr = (const char*)lib::singleton().p_sqlite3_column_text(m_stmt, i);
      row[i] = std::string(text_ptr? text_ptr: "");
      }
      break;

    case SQLITE_BLOB:
      row[i] = blob_t();
      brig::blob_t& blob = ::boost::get<brig::blob_t>(row[i]);
      if (m_cols[i].geometry)
        column_geometry(m_stmt, i, blob);
      else
      {
        blob.resize(lib::singleton().p_sqlite3_column_bytes(m_stmt, i));
        if (!blob.empty()) memcpy(blob.data(), lib::singleton().p_sqlite3_column_blob(m_stmt, i), blob.size());
      }
      break;
    }

  switch (lib::singleton().p_sqlite3_step(m_stmt))
  {
  default: m_db.error(); return false;
  case SQLITE_ROW: return true;
  case SQLITE_DONE: m_done = true; return true;
  }
}

inline void command::set_autocommit(bool autocommit)
{
  close_stmt();
  if (m_autocommit == autocommit) return;
  exec(autocommit? "ROLLBACK": "BEGIN");
  m_autocommit = autocommit;
}

inline void command::commit()
{
  close_stmt();
  if (m_autocommit) return;
  exec("COMMIT");
  exec("BEGIN");
} // command::

} } } } // brig::database::sqlite::detail

#endif // BRIG_DATABASE_SQLITE_DETAIL_COMMAND_HPP
