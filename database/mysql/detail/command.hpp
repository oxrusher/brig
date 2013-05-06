// Andrew Naplavkov

#ifndef BRIG_DATABASE_MYSQL_DETAIL_COMMAND_HPP
#define BRIG_DATABASE_MYSQL_DETAIL_COMMAND_HPP

#include <algorithm>
#include <boost/ptr_container/ptr_vector.hpp>
#include <brig/database/command.hpp>
#include <brig/database/mysql/detail/bind_param.hpp>
#include <brig/database/mysql/detail/bind_result_factory.hpp>
#include <brig/database/mysql/detail/lib.hpp>
#include <brig/global.hpp>
#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

namespace brig { namespace database { namespace mysql { namespace detail {

class command : public brig::database::command {
  MYSQL* m_con;
  MYSQL_STMT* m_stmt;
  std::vector<MYSQL_BIND> m_binds;
  ::boost::ptr_vector<bind_result> m_cols;
  bool m_autocommit;

  void check(bool r);
  void close_stmt();
  void close_all();

public:
  command(const std::string& host, int port, const std::string& db, const std::string& usr, const std::string& pwd);
  ~command() override  { close_all(); }
  void exec(const std::string& sql, const std::vector<column_def>& params = std::vector<column_def>()) override;
  void exec_batch(const std::string& sql) override;
  std::vector<std::string> columns() override;
  bool fetch(std::vector<variant>& row) override;
  void set_autocommit(bool autocommit) override;
  void commit() override;
  DBMS system() override  { return DBMS::MySQL; }
}; // command

inline void command::check(bool r)
{
  if (r) return;
  std::string msg(lib::singleton().p_mysql_error(m_con));
  if (msg.empty()) msg = "MySQL error";
  throw std::runtime_error(msg);
}

inline void command::close_stmt()
{
  if (!m_stmt) return;
  m_binds.clear();
  m_cols.clear();
  MYSQL_STMT* stmt(0); std::swap(stmt, m_stmt);
  lib::singleton().p_mysql_stmt_close(stmt);
}

inline void command::close_all()
{
  m_autocommit = true;
  close_stmt();
  if (!m_con) return;
  MYSQL* con(0); std::swap(con, m_con);
  lib::singleton().p_mysql_close(con);
}

inline command::command(const std::string& host, int port, const std::string& db, const std::string& usr, const std::string& pwd)
  : m_con(0), m_stmt(0), m_autocommit(true)
{
  if (lib::singleton().empty()) throw std::runtime_error("MySQL error");
  m_con = lib::singleton().p_mysql_init(0);
  if (!m_con) throw std::runtime_error("MySQL error");
  try
  {
    check(lib::singleton().p_mysql_real_connect(m_con, host.c_str(), usr.c_str(), pwd.c_str(), db.c_str(), port, 0, CLIENT_MULTI_STATEMENTS) == m_con);
    check(lib::singleton().p_mysql_set_character_set(m_con, "utf8") == 0);
  }
  catch (const std::exception&)  { close_all(); throw; }
}

inline void command::exec(const std::string& sql, const std::vector<column_def>& params)
{
  close_stmt();
  m_stmt = lib::singleton().p_mysql_stmt_init(m_con);
  if (!m_stmt) throw std::runtime_error("MySQL error");
  check(lib::singleton().p_mysql_stmt_prepare(m_stmt, sql.c_str(), (unsigned long)sql.size()) == 0);

  std::vector<MYSQL_BIND> binds(params.size());
  if (!binds.empty())
  {
    memset(binds.data(), 0, binds.size() * sizeof(MYSQL_BIND));
    for (size_t i(0); i < params.size(); ++i)
      bind_param(params[i].query_value, binds[i]);
    check(lib::singleton().p_mysql_stmt_bind_param(m_stmt, binds.data()) == 0);
  }

  check(lib::singleton().p_mysql_stmt_execute(m_stmt) == 0);
}

inline void command::exec_batch(const std::string& sql)
{
  close_stmt();
  check(lib::singleton().p_mysql_query(m_con, sql.c_str()) == 0);
  while (true)
  {
    MYSQL_RES* res(lib::singleton().p_mysql_store_result(m_con));
    if (res) lib::singleton().p_mysql_free_result(res);
    const int r(lib::singleton().p_mysql_next_result(m_con));
    if (r < 0) break;
    check(r == 0);
  }
}

inline std::vector<std::string> command::columns()
{
  std::vector<std::string> cols;
  if (!m_stmt) return cols;
  m_binds.clear();
  m_cols.clear();

  MYSQL_RES* res(lib::singleton().p_mysql_stmt_result_metadata(m_stmt));
  check(res != 0);
  const unsigned int count(lib::singleton().p_mysql_num_fields(res));
  m_binds.resize(count);
  memset(m_binds.data(), 0, m_binds.size() * sizeof(MYSQL_BIND));
  for (unsigned int i(0); i < count; ++i)
  {
    MYSQL_FIELD* field(lib::singleton().p_mysql_fetch_field_direct(res, i));
    m_cols.push_back(bind_result_factory(field, m_binds[i]));
    cols.push_back(field->name);
  }
  lib::singleton().p_mysql_free_result(res);
  check(lib::singleton().p_mysql_stmt_bind_result(m_stmt, m_binds.data()) == 0);
  return cols;
}

inline bool command::fetch(std::vector<variant>& row)
{
  if (!m_stmt) return false;
  if (m_cols.empty()) columns();

  const int r(lib::singleton().p_mysql_stmt_fetch(m_stmt));
  if (MYSQL_NO_DATA == r) return false;
  check(1 != r);

  row.resize(m_cols.size());
  for (size_t i(0); i < m_cols.size(); ++i)
    check(m_cols[i](m_stmt, (unsigned int)i, row[i]) == 0);
  return true;
}

inline void command::set_autocommit(bool autocommit)
{
  close_stmt();
  if (m_autocommit == autocommit) return;
  check(lib::singleton().p_mysql_query(m_con, autocommit? "ROLLBACK": "BEGIN") == 0);
  m_autocommit = autocommit;
}

inline void command::commit()
{
  close_stmt();
  if (m_autocommit) return;
  check(lib::singleton().p_mysql_query(m_con, "COMMIT") == 0);
  check(lib::singleton().p_mysql_query(m_con, "BEGIN") == 0);
} // command::

} } } } // brig::database::mysql::detail

#endif // BRIG_DATABASE_MYSQL_DETAIL_COMMAND_HPP
