// Andrew Naplavkov

#ifndef BRIG_DATABASE_CUBRID_DETAIL_COMMAND_HPP
#define BRIG_DATABASE_CUBRID_DETAIL_COMMAND_HPP

#include <algorithm>
#include <boost/ptr_container/ptr_vector.hpp>
#include <brig/database/command.hpp>
#include <brig/database/cubrid/detail/binding.hpp>
#include <brig/database/cubrid/detail/get_data_factory.hpp>
#include <brig/database/cubrid/detail/lib.hpp>
#include <stdexcept>
#include <string>

namespace brig { namespace database { namespace cubrid { namespace detail {

class command : public brig::database::command {
  int m_con, m_req;
  T_CCI_ERROR m_err;
  ::boost::ptr_vector<get_data> m_cols;

  void check(int r);
  void close_request();

public:
  command(const std::string& url, const std::string& usr, const std::string& pwd);
  ~command() override;
  void exec(const std::string& sql, const std::vector<column_def>& params = std::vector<column_def>()) override;
  void exec_batch(const std::string& sql) override;
  std::vector<std::string> columns() override;
  bool fetch(std::vector<variant>& row) override;
  void set_autocommit(bool autocommit) override;
  void commit() override;
  DBMS system() override  { return CUBRID; }
}; // command

inline void command::check(int r)
{
  if (!lib::error(r)) return;
  else if (m_err.err_msg[0]) throw std::runtime_error(m_err.err_msg);
  else throw std::runtime_error("CUBRID error");
}

inline void command::close_request()
{
  if (lib::error(m_req)) return;
  m_cols.clear();
  int req(CCI_ER_REQ_HANDLE); std::swap(m_req, req);
  lib::singleton().p_cci_close_req_handle(req);
}

inline command::~command()
{
  close_request();
  if (lib::error(m_con)) return;
  int con(CCI_ER_CON_HANDLE); std::swap(m_con, con);
  lib::singleton().p_cci_disconnect(con, &m_err);
}

inline command::command(const std::string& url, const std::string& usr, const std::string& pwd)
  : m_con(CCI_ER_CON_HANDLE), m_req(CCI_ER_REQ_HANDLE)
{
  if (lib::singleton().empty()) throw std::runtime_error("CUBRID error");
  int con(lib::singleton().p_cci_connect_with_url((char*)url.c_str(), (char*)usr.c_str(), (char*)pwd.c_str()));
  if (lib::error(con)) throw std::runtime_error("CUBRID error");
  std::swap(m_con, con);
}

inline void command::exec(const std::string& sql, const std::vector<column_def>& params)
{
  close_request();
  int req(lib::singleton().p_cci_prepare(m_con, (char*)sql.c_str(), 0, &m_err));
  check(req);
  std::swap(m_req, req);

  for (size_t i(0); i < params.size(); ++i)
    if (lib::error(bind(m_req, i, params[i]))) throw std::runtime_error("CUBRID error");

  check(lib::singleton().p_cci_execute(m_req, CCI_EXEC_QUERY_ALL, 0, &m_err));
}

inline void command::exec_batch(const std::string& sql)
{
  exec(sql);
  while (true)
  {
    const int res(lib::singleton().p_cci_next_result(m_req, &m_err));
    if (CAS_ER_NO_MORE_RESULT_SET == res) break;
    check(res);
  }
  close_request();
}

inline std::vector<std::string> command::columns()
{
  std::vector<std::string> cols;
  if (lib::error(m_req)) return cols;

  m_cols.clear();
  T_CCI_SQLX_CMD cmd_type;
  int col_count(0);
  T_CCI_COL_INFO* col_info(lib::singleton().p_cci_get_result_info(m_req, &cmd_type, &col_count));
  if (!col_info) throw std::runtime_error("CUBRID error");

  for (int i(1); i <= col_count; ++i)
  {
    m_cols.push_back(get_data_factory(CCI_GET_RESULT_INFO_TYPE(col_info, i)));
    cols.push_back(CCI_GET_RESULT_INFO_NAME(col_info, i));
  }
  return cols;
}

inline bool command::fetch(std::vector<variant>& row)
{
  if (lib::error(m_req)) return false;
  if (m_cols.empty()) columns();

  const int r(lib::singleton().p_cci_cursor(m_req, 1, CCI_CURSOR_CURRENT, &m_err));
  if (CCI_ER_NO_MORE_DATA == r) return false;
  check(r);
  check(lib::singleton().p_cci_fetch(m_req, &m_err));

  row.resize(m_cols.size());
  for (size_t i(0); i < m_cols.size(); ++i)
    if (lib::error(m_cols[i](m_req, i, row[i]))) throw std::runtime_error("CUBRID error");
  return true;
}

inline void command::set_autocommit(bool autocommit)
{
  close_request();
  if (lib::singleton().p_cci_get_autocommit(m_con) == (autocommit? 1: 0)) return;
  if (autocommit) check(lib::singleton().p_cci_end_tran(m_con, CCI_TRAN_ROLLBACK, &m_err));
  if (lib::error(lib::singleton().p_cci_set_autocommit(m_con, autocommit? CCI_AUTOCOMMIT_TRUE: CCI_AUTOCOMMIT_FALSE))) throw std::runtime_error("CUBRID error");
}

inline void command::commit()
{
  close_request();
  if (lib::singleton().p_cci_get_autocommit(m_con)) return;
  check(lib::singleton().p_cci_end_tran(m_con, CCI_TRAN_COMMIT, &m_err));
} // command::

} } } } // brig::database::cubrid::detail

#endif // BRIG_DATABASE_CUBRID_DETAIL_COMMAND_HPP
