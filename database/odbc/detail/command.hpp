// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_DETAIL_COMMAND_HPP
#define BRIG_DATABASE_ODBC_DETAIL_COMMAND_HPP

#include <algorithm>
#include <boost/ptr_container/ptr_vector.hpp>
#include <brig/database/command.hpp>
#include <brig/database/odbc/detail/binding_factory.hpp>
#include <brig/database/odbc/detail/get_data_factory.hpp>
#include <brig/database/odbc/detail/lib.hpp>
#include <brig/unicode/lower_case.hpp>
#include <brig/unicode/transform.hpp>
#include <stdexcept>
#include <string>

namespace brig { namespace database { namespace odbc { namespace detail {

class command : public brig::database::command {
  SQLHANDLE m_env, m_dbc, m_stmt;
  DBMS m_sys;
  std::string m_sql;
  ::boost::ptr_vector<get_data> m_cols;

  void close_stmt();
  void close_all();
  void check(SQLSMALLINT type, SQLHANDLE handle, SQLRETURN r);
  bool get_autocommit();

public:
  command(const std::string& str);
  ~command() override  { close_all(); }
  void exec(const std::string& sql, const std::vector<column_def>& params = std::vector<column_def>()) override;
  void exec_batch(const std::string& sql) override;
  std::vector<std::string> columns() override;
  bool fetch(std::vector<variant>& row) override;
  void set_autocommit(bool autocommit) override;
  void commit() override;
  DBMS system() override  { return m_sys; }
}; // command

inline void command::close_stmt()
{
  if (SQL_NULL_HANDLE == m_stmt) return;
  m_cols.clear();
  m_sql = "";
  SQLHANDLE stmt(SQL_NULL_HANDLE); std::swap(stmt, m_stmt);
  lib::singleton().p_SQLFreeStmt(stmt, SQL_CLOSE); // Postgres hangs on under Visual Studio IDE
  lib::singleton().p_SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}

inline void command::close_all()
{
  using namespace std;

  close_stmt();
  m_sys = DBMS::Void;
  if (SQL_NULL_HANDLE != m_dbc)
  {
    SQLHANDLE dbc(SQL_NULL_HANDLE); swap(dbc, m_dbc);
    lib::singleton().p_SQLDisconnect(dbc);
    lib::singleton().p_SQLFreeHandle(SQL_HANDLE_DBC, dbc);
  }
  if (SQL_NULL_HANDLE != m_env)
  {
    SQLHANDLE env(SQL_NULL_HANDLE); swap(env, m_env);
    lib::singleton().p_SQLFreeHandle(SQL_HANDLE_ENV, env);
  }
}

inline void command::check(SQLSMALLINT type, SQLHANDLE handle, SQLRETURN r)
{
  using namespace std;

  if (SQL_SUCCEEDED(r)) return;
  basic_string<SQLWCHAR> msg;
  if (SQL_NULL_HANDLE != handle  && SQL_ERROR == r)
  {
    SQLWCHAR state[6], buf[SQL_MAX_MESSAGE_LENGTH];
    SQLINTEGER err(0);
    SQLSMALLINT row(1), len(0);
    while (SQL_SUCCEEDED(lib::singleton().p_SQLGetDiagRecW(type, handle, row++, state, &err, buf, SQL_MAX_MESSAGE_LENGTH, &len)))
    {
      if (!msg.empty()) msg += SQLWCHAR(32);
      msg += buf;
    }
  }
  throw runtime_error(msg.empty()? "ODBC error": brig::unicode::transform<char>(msg));
}

inline command::command(const std::string& str) : m_env(SQL_NULL_HANDLE), m_dbc(SQL_NULL_HANDLE), m_stmt(SQL_NULL_HANDLE), m_sys(DBMS::Void)
{
  using namespace std;
  using namespace brig::unicode;

  SQLWCHAR buf[SQL_MAX_MESSAGE_LENGTH];
  SQLSMALLINT len(0);

  // environment
  SQLHANDLE env(SQL_NULL_HANDLE);
  if (lib::singleton().empty() || !SQL_SUCCEEDED(lib::singleton().p_SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env))) throw runtime_error("ODBC error");
  swap(m_env, env);

  // connection
  try
  {
    SQLHANDLE dbc(SQL_NULL_HANDLE);
    check(SQL_HANDLE_ENV, m_env, lib::singleton().p_SQLSetEnvAttr(m_env, SQL_ATTR_ODBC_VERSION, SQLPOINTER(SQL_OV_ODBC3), 0));
    check(SQL_HANDLE_ENV, m_env, lib::singleton().p_SQLAllocHandle(SQL_HANDLE_DBC, m_env, &dbc));
    swap(m_dbc, dbc);

    check(SQL_HANDLE_DBC, m_dbc, lib::singleton().p_SQLDriverConnectW
      ( m_dbc, 0
      , (SQLWCHAR*)transform<SQLWCHAR>(str).c_str(), SQL_NTS
      , 0, 0, &len, SQL_DRIVER_NOPROMPT
      ));
  }
  catch (const exception&)  { close_all(); throw; }
  
  // DBMS
  if (SQL_SUCCEEDED(lib::singleton().p_SQLGetInfoW(m_dbc, SQL_DBMS_NAME, buf, SQL_MAX_MESSAGE_LENGTH, &len)))
  {
    const string sys(transform<char>(buf, lower_case));
         if (sys.find("cubrid") != string::npos) m_sys = DBMS::CUBRID;
    else if (sys.find("db2") != string::npos) m_sys = DBMS::DB2;
    else if (sys.find("informix") != string::npos) m_sys = DBMS::Informix;
    else if (sys.find("ingres") != string::npos) m_sys = DBMS::Ingres;
    else if (sys.find("microsoft") != string::npos
          && sys.find("sql") != string::npos
          && sys.find("server") != string::npos) m_sys = DBMS::MS_SQL;
    else if (sys.find("mysql") != string::npos) m_sys = DBMS::MySQL;
    else if (sys.find("oracle") != string::npos) m_sys = DBMS::Oracle;
    else if (sys.find("postgres") != string::npos) m_sys = DBMS::Postgres;
    else if (sys.find("sqlite") != string::npos) m_sys = DBMS::SQLite;
  }

  // http://www.ibm.com/Search/?q=Delimited+Identifiers+in+ODBC
  if (DBMS::Informix == m_sys)
  {
    try
    {
      SQLINTEGER attr(0);
      SQLINTEGER len(sizeof(attr));
      check(SQL_HANDLE_DBC, m_dbc, lib::singleton().p_SQLGetConnectAttr(m_dbc, SQL_INFX_ATTR_DELIMIDENT, &attr, sizeof(attr), &len));
      if (SQL_TRUE != attr) throw runtime_error("Enable delimited identifiers (DELIMIDENT=y)");
    }
    catch (const exception&)  { close_all(); throw; }
  }
}

inline void command::exec(const std::string& sql, const std::vector<column_def>& params)
{
  if (SQL_NULL_HANDLE == m_stmt || sql != m_sql || sql.empty())
  {
    close_stmt();
    SQLHANDLE stmt(SQL_NULL_HANDLE);
    check(SQL_HANDLE_DBC, m_dbc, lib::singleton().p_SQLAllocHandle(SQL_HANDLE_STMT, m_dbc, &stmt));
    std::swap(m_stmt, stmt);
    check(SQL_HANDLE_STMT, m_stmt, lib::singleton().p_SQLPrepareW(m_stmt, (SQLWCHAR*)brig::unicode::transform<SQLWCHAR>(sql).c_str(), SQL_NTS));
    m_sql = sql;
  }
  else
  {
    lib::singleton().p_SQLFreeStmt(m_stmt, SQL_CLOSE);
    m_cols.clear();
  }

  ::boost::ptr_vector<binding> binds;
  for (size_t i(0); i < params.size(); ++i)
  {
    binding* bind(binding_factory(m_sys, params[i]));
    binds.push_back(bind);
    check(SQL_HANDLE_STMT, m_stmt, lib::singleton().p_SQLBindParameter(m_stmt, SQLUSMALLINT(i + 1), SQL_PARAM_INPUT
      , bind->c_type(), bind->sql_type(), bind->column_size(), 0, bind->val_ptr(), 0, bind->ind()));
  }
  const SQLRETURN r(lib::singleton().p_SQLExecute(m_stmt));
  if (SQL_NO_DATA != r) check(SQL_HANDLE_STMT, m_stmt, r);
}

inline void command::exec_batch(const std::string& sql)
{
  exec(sql);
  while (true)
  {
    const SQLRETURN r(lib::singleton().p_SQLMoreResults(m_stmt));
    if (SQL_NO_DATA == r) break;
    check(SQL_HANDLE_STMT, m_stmt, r);
  }
  close_stmt();
}

inline std::vector<std::string> command::columns()
{
  using namespace std;

  vector<string> cols;
  if (SQL_NULL_HANDLE == m_stmt) return cols;
  m_sql = "";
  m_cols.clear();

  SQLSMALLINT count(0);
  check(SQL_HANDLE_STMT, m_stmt, lib::singleton().p_SQLNumResultCols(m_stmt, &count));
  for (SQLSMALLINT i(0); i < count; ++i)
  {
    SQLUSMALLINT col = SQLUSMALLINT(i + 1);
    SQLWCHAR buf[SQL_MAX_MESSAGE_LENGTH];
    SQLSMALLINT len(0);
    SQLLEN sql_type(SQL_UNKNOWN_TYPE);
    SQLLEN precision(0), scale(0);

    check(SQL_HANDLE_STMT, m_stmt, lib::singleton().p_SQLColAttributeW(m_stmt, col, SQL_DESC_NAME, &buf, SQL_MAX_MESSAGE_LENGTH, &len, 0));
    check(SQL_HANDLE_STMT, m_stmt, lib::singleton().p_SQLColAttributeW(m_stmt, col, SQL_DESC_TYPE, 0, 0, 0, &sql_type));
    if ( (SQL_DECIMAL == sql_type || SQL_NUMERIC == sql_type)
      && SQL_SUCCEEDED(lib::singleton().p_SQLColAttributeW(m_stmt, col, SQL_DESC_PRECISION, 0, 0, 0, &precision))
      && SQL_SUCCEEDED(lib::singleton().p_SQLColAttributeW(m_stmt, col, SQL_DESC_SCALE, 0, 0, 0, &scale))
      && 0 < precision
      && 0 == scale)
    {
      if (precision <= 5) sql_type = SQL_SMALLINT;
      else if (precision <= 10) sql_type = SQL_INTEGER;
      else if (precision <= 19) sql_type = SQL_BIGINT;
    }

    m_cols.push_back(get_data_factory(SQLSMALLINT(sql_type)));
    cols.push_back(brig::unicode::transform<char>(buf));
  }
  return cols;
}

inline bool command::fetch(std::vector<variant>& row)
{
  if (SQL_NULL_HANDLE == m_stmt) return false;
  if (m_cols.empty()) columns();

  const SQLRETURN r(lib::singleton().p_SQLFetch(m_stmt));
  if (SQL_NO_DATA == r) return false;
  check(SQL_HANDLE_STMT, m_stmt, r);

  row.resize(m_cols.size());
  for (size_t i(0); i < m_cols.size(); ++i)
    check(SQL_HANDLE_STMT, m_stmt, m_cols[i](m_stmt, i, row[i]));
  return true;
}

inline bool command::get_autocommit()
{
  SQLUINTEGER attr(0);
  SQLINTEGER len(sizeof(attr));
  check(SQL_HANDLE_DBC, m_dbc, lib::singleton().p_SQLGetConnectAttr(m_dbc, SQL_ATTR_AUTOCOMMIT, &attr, sizeof(attr), &len));
  return SQL_AUTOCOMMIT_ON == attr;
}

inline void command::set_autocommit(bool autocommit)
{
  close_stmt();
  if (get_autocommit() == autocommit) return;
  if (autocommit) check(SQL_HANDLE_DBC, m_dbc, lib::singleton().p_SQLEndTran(SQL_HANDLE_DBC, m_dbc, SQL_ROLLBACK));
  check(SQL_HANDLE_DBC, m_dbc, lib::singleton().p_SQLSetConnectAttr(m_dbc, SQL_ATTR_AUTOCOMMIT, SQLPOINTER(autocommit? SQL_AUTOCOMMIT_ON: SQL_AUTOCOMMIT_OFF), 0));
}

inline void command::commit()
{
  close_stmt();
  if (get_autocommit()) return;
  check(SQL_HANDLE_DBC, m_dbc, lib::singleton().p_SQLEndTran(SQL_HANDLE_DBC, m_dbc, SQL_COMMIT));
} // command::

} } } } // brig::database::odbc::detail

#endif // BRIG_DATABASE_ODBC_DETAIL_COMMAND_HPP
