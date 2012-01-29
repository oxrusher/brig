// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_DETAIL_LINK_HPP
#define BRIG_DATABASE_ODBC_DETAIL_LINK_HPP

#include <boost/algorithm/string.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <brig/database/link.hpp>
#include <brig/database/odbc/detail/binding_factory.hpp>
#include <brig/database/odbc/detail/get_data_factory.hpp>
#include <brig/database/odbc/detail/lib.hpp>
#include <brig/unicode/transform.hpp>
#include <stdexcept>
#include <string>

namespace brig { namespace database { namespace odbc { namespace detail {

class link : public brig::database::link {
  SQLHANDLE m_env, m_dbc, m_stmt;
  DBMS m_sys;
  boost::ptr_vector<get_data> m_cols;
  std::string m_sql;

  void close_stmt();
  void close_all();
  void error();
  void check(SQLSMALLINT type, SQLHANDLE handle, SQLRETURN r);

public:
  link(const std::string& str);
  virtual ~link()  { close_all(); }
  virtual DBMS system()  { return m_sys; }
  virtual void exec
    ( const std::string& sql
    , const std::vector<variant>& params = std::vector<variant>()
    , const std::vector<column_detail>& param_cols = std::vector<column_detail>()
    );
  virtual int64_t affected();
  virtual void columns(std::vector<std::string>& cols);
  virtual bool fetch(std::vector<variant>& row);
  virtual void start();
  virtual void commit();
  virtual void rollback();
}; // link

inline void link::close_stmt()
{
  if (SQL_NULL_HANDLE == m_stmt) return;
  m_sql = "";
  m_cols.clear();
  SQLHANDLE stmt(SQL_NULL_HANDLE); std::swap(stmt, m_stmt);
  lib::singleton().p_SQLFreeStmt(stmt, SQL_CLOSE); // Postgres hangs on under Visual Studio IDE
  lib::singleton().p_SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}

inline void link::close_all()
{
  close_stmt();
  m_sys = UnknownSystem;
  if (m_dbc != SQL_NULL_HANDLE)
  {
    SQLHANDLE dbc(SQL_NULL_HANDLE); std::swap(dbc, m_dbc);
    lib::singleton().p_SQLDisconnect(dbc);
    lib::singleton().p_SQLFreeHandle(SQL_HANDLE_DBC, dbc);
  }
  if (m_env != SQL_NULL_HANDLE)
  {
    SQLHANDLE env(SQL_NULL_HANDLE); std::swap(env, m_env);
    lib::singleton().p_SQLFreeHandle(SQL_HANDLE_ENV, env);
  }
}

inline void link::error()
{
  close_stmt();
  throw std::runtime_error("ODBC error");
}

inline void link::check(SQLSMALLINT type, SQLHANDLE handle, SQLRETURN r)
{
  if (SQL_SUCCEEDED(r)) return;

  std::basic_string<SQLWCHAR> msg;
  if (handle != SQL_NULL_HANDLE && (SQL_SUCCESS_WITH_INFO == r || SQL_ERROR == r))
  {
    SQLWCHAR state[6], buf[SQL_MAX_MESSAGE_LENGTH];
    SQLINTEGER err(0);
    SQLSMALLINT row(1), len(0);
    while (true)
    {
      if (!SQL_SUCCEEDED(lib::singleton().p_SQLGetDiagRecW(type, handle, row++, state, &err, buf, SQL_MAX_MESSAGE_LENGTH, &len))) break;
      if (!msg.empty()) msg += SQLWCHAR(32);
      msg += buf;
    }
  }

  close_stmt();
  throw std::runtime_error(msg.empty()? "ODBC error": brig::unicode::transform<std::string>(msg));
}

inline link::link(const std::string& str) : m_env(SQL_NULL_HANDLE), m_dbc(SQL_NULL_HANDLE), m_stmt(SQL_NULL_HANDLE), m_sys(UnknownSystem)
{
  SQLWCHAR buf[SQL_MAX_MESSAGE_LENGTH];
  SQLSMALLINT len(0);

  // environment
  SQLHANDLE env(SQL_NULL_HANDLE);
  if (lib::singleton().empty() || !SQL_SUCCEEDED(lib::singleton().p_SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env))) error();
  std::swap(m_env, env);

  // connection
  try
  {
    SQLHANDLE dbc(SQL_NULL_HANDLE);
    check(SQL_HANDLE_ENV, m_env, lib::singleton().p_SQLSetEnvAttr(m_env, SQL_ATTR_ODBC_VERSION, SQLPOINTER(SQL_OV_ODBC3), 0));
    check(SQL_HANDLE_ENV, m_env, lib::singleton().p_SQLAllocHandle(SQL_HANDLE_DBC, m_env, &dbc));
    std::swap(m_dbc, dbc);

    check(SQL_HANDLE_DBC, m_dbc, lib::singleton().p_SQLDriverConnectW
      ( m_dbc, 0
      , (SQLWCHAR*)brig::unicode::transform<std::basic_string<SQLWCHAR>>(str).c_str(), SQL_NTS
      , 0, 0, &len, SQL_DRIVER_NOPROMPT
      ));
  }
  catch (const std::exception&)
  {
    close_all();
    throw;
  }
  
  // DBMS
  if (SQL_SUCCEEDED(lib::singleton().p_SQLGetInfoW(m_dbc, SQL_DBMS_NAME, buf, SQL_MAX_MESSAGE_LENGTH, &len)))
  {
    std::string sys(brig::unicode::transform<std::string>(buf));
    boost::algorithm::to_upper(sys);
         if (sys.find("DB2") != std::string::npos) m_sys = DB2;
    else if (sys.find("MICROSOFT") != std::string::npos
          && sys.find("SQL") != std::string::npos
          && sys.find("SERVER") != std::string::npos) m_sys = MS_SQL;
    else if (sys.find("MYSQL") != std::string::npos) m_sys = MySQL;
    else if (sys.find("ORACLE") != std::string::npos) m_sys = Oracle;
    else if (sys.find("POSTGRES") != std::string::npos) m_sys = Postgres;
    else if (sys.find("SQLITE") != std::string::npos) m_sys = SQLite;
  }
}

inline void link::exec(const std::string& sql, const std::vector<variant>& params, const std::vector<column_detail>& param_cols)
{
  if (SQL_NULL_HANDLE == m_stmt || sql != m_sql)
  {
    close_stmt();
    SQLHANDLE stmt(SQL_NULL_HANDLE);
    check(SQL_HANDLE_DBC, m_dbc, lib::singleton().p_SQLAllocHandle(SQL_HANDLE_STMT, m_dbc, &stmt));
    std::swap(m_stmt, stmt);
    check(SQL_HANDLE_STMT, m_stmt, lib::singleton().p_SQLPrepareW(m_stmt, (SQLWCHAR*)brig::unicode::transform<std::basic_string<SQLWCHAR>>(sql).c_str(), SQL_NTS));
    m_sql = sql;
  }
  else lib::singleton().p_SQLFreeStmt(m_stmt, SQL_CLOSE);

  boost::ptr_vector<binding> binds;
  for (size_t i(0); i < params.size(); ++i)
  {
    binding* bind(binding_factory(m_sys, params[i], i < param_cols.size()? &param_cols[i]: 0));
    binds.push_back(bind);
    check(SQL_HANDLE_STMT, m_stmt, lib::singleton().p_SQLBindParameter(m_stmt, SQLUSMALLINT(i + 1), SQL_PARAM_INPUT
      , bind->c_type(), bind->sql_type(), bind->precision(), 0, bind->val_ptr(), 0, bind->ind()));
  }
  const SQLRETURN r(lib::singleton().p_SQLExecute(m_stmt));
  if (SQL_NO_DATA != r) check(SQL_HANDLE_STMT, m_stmt, r);
}

inline int64_t link::affected()
{
  SQLLEN count(0);
  if (SQL_NULL_HANDLE != m_stmt) check(SQL_HANDLE_STMT, m_stmt, lib::singleton().p_SQLRowCount(m_stmt, &count));
  if (count < 0) count = 0;
  return count;
}

inline void link::columns(std::vector<std::string>& cols)
{
  if (SQL_NULL_HANDLE == m_stmt) return;
  m_cols.clear();
  m_sql = "";

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
    cols.push_back(brig::unicode::transform<std::string>(buf));
  }
}

inline bool link::fetch(std::vector<variant>& row)
{
  if (SQL_NULL_HANDLE == m_stmt) return false;
  if (m_cols.empty())  { std::vector<std::string> cols; columns(cols); }

  const SQLRETURN r(lib::singleton().p_SQLFetch(m_stmt));
  if (SQL_NO_DATA == r) return false;
  check(SQL_HANDLE_STMT, m_stmt, r);

  row.resize(m_cols.size());
  for (size_t i(0); i < m_cols.size(); ++i)
    check(SQL_HANDLE_STMT, m_stmt, m_cols[i](m_stmt, i, row[i]));
  return true;
}

inline void link::start()
{
  close_stmt();
  check(SQL_HANDLE_DBC, m_dbc, lib::singleton().p_SQLSetConnectAttr(m_dbc,  SQL_ATTR_AUTOCOMMIT, SQLPOINTER(SQL_AUTOCOMMIT_OFF), 0));
}

inline void link::commit()
{
  close_stmt();
  check(SQL_HANDLE_DBC, m_dbc, lib::singleton().p_SQLEndTran(SQL_HANDLE_DBC, m_dbc, SQL_COMMIT));
  check(SQL_HANDLE_DBC, m_dbc, lib::singleton().p_SQLSetConnectAttr(m_dbc,  SQL_ATTR_AUTOCOMMIT, SQLPOINTER(SQL_AUTOCOMMIT_ON), 0));
}

inline void link::rollback()
{
  close_stmt();
  check(SQL_HANDLE_DBC, m_dbc, lib::singleton().p_SQLEndTran(SQL_HANDLE_DBC, m_dbc, SQL_ROLLBACK));
  check(SQL_HANDLE_DBC, m_dbc, lib::singleton().p_SQLSetConnectAttr(m_dbc,  SQL_ATTR_AUTOCOMMIT, SQLPOINTER(SQL_AUTOCOMMIT_ON), 0));
} // link::

} } } } // brig::database::odbc::detail

#endif // BRIG_DATABASE_ODBC_DETAIL_LINK_HPP
