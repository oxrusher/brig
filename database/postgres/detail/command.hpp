// Andrew Naplavkov

#ifndef BRIG_DATABASE_POSTGRES_DETAIL_COMMAND_HPP
#define BRIG_DATABASE_POSTGRES_DETAIL_COMMAND_HPP

#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <brig/database/command.hpp>
#include <brig/database/postgres/detail/binding_factory.hpp>
#include <brig/database/postgres/detail/get_value_factory.hpp>
#include <brig/database/postgres/detail/lib.hpp>
#include <brig/global.hpp>
#include <brig/identifier.hpp>
#include <brig/string_cast.hpp>
#include <brig/unicode/lower_case.hpp>
#include <brig/unicode/transform.hpp>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>

namespace brig { namespace database { namespace postgres { namespace detail {

class command : public brig::database::command {
  PGconn* m_con;
  PGresult* m_res;
  bool m_fetch;
  ::boost::ptr_vector<get_value> m_cols;
  int m_row;
  bool m_autocommit;

  void check(bool r);
  void check_command(PGresult* res);
  void close_result();

public:
  command(const std::string& host, int port, const std::string& db, const std::string& usr, const std::string& pwd);
  ~command() override;
  void exec(const std::string& sql, const std::vector<column_def>& params = std::vector<column_def>()) override;
  void exec_batch(const std::string& sql) override;
  std::vector<std::string> columns() override;
  bool fetch(std::vector<variant>& row) override;
  void set_autocommit(bool autocommit) override;
  void commit() override;
  DBMS system() override  { return Postgres; }
  std::string sql_param(size_t order) override  { return "$" + string_cast<char>(order + 1); }
}; // command

inline void command::check(bool r)
{
  if (r) return;
  std::string msg(lib::singleton().p_PQerrorMessage(m_con));
  if (msg.empty()) msg = "Postgres error";
  throw std::runtime_error(msg);
}

inline void command::check_command(PGresult* res)
{
  const ExecStatusType r(lib::singleton().p_PQresultStatus(res));
  lib::singleton().p_PQclear(res);
  check(r == PGRES_COMMAND_OK);
}

inline void command::close_result()
{
  m_row = 0;
  m_cols.clear();
  if (m_res)
  {
    PGresult* res(0); std::swap(res, m_res);
    lib::singleton().p_PQclear(res);
  }
  if (m_fetch)
  {
    m_fetch = false;
    lib::singleton().p_PQclear(lib::singleton().p_PQexec(m_con, "CLOSE BrigCursor; END;"));
  }
}

inline command::command(const std::string& host, int port, const std::string& db, const std::string& usr, const std::string& pwd)
  : m_con(0), m_res(0), m_fetch(false), m_row(0), m_autocommit(true)
{
  if (lib::singleton().empty()) throw std::runtime_error("Postgres error");
  const std::string con("host='" + host + "' port='" + string_cast<char>(port) + "' dbname='" + db + "' user='" + usr + "' password='" + pwd + "' connect_timeout='" + string_cast<char>(TimeoutSec) + "' client_encoding='UTF8'");
  m_con = lib::singleton().p_PQconnectdb((char*)con.c_str());
  check(lib::singleton().p_PQstatus(m_con) == CONNECTION_OK);
}

inline command::~command()
{
  close_result();
  lib::singleton().p_PQfinish(m_con);
}

inline void command::exec(const std::string& sql, const std::vector<column_def>& params)
{
  using namespace std;

  close_result();

  ::boost::ptr_vector<binding> binds;
  vector<Oid> types;
  vector<char*> values;
  vector<int> lengths;
  vector<int> formats;
  for (size_t i(0); i < params.size(); ++i)
  {
    binding* bind(binding_factory(params[i].query_value));
    binds.push_back(bind);
    types.push_back(bind->type());
    values.push_back((char*)bind->value());
    lengths.push_back(bind->length());
    formats.push_back(bind->format());
  }

  if (m_autocommit && sql.size() > 6 && brig::unicode::transform<char>(sql.substr(0, 6), brig::unicode::lower_case).compare("select") == 0)
  {
    check_command(lib::singleton().p_PQexec(m_con, "BEGIN"));
    m_fetch = true;
    check_command(lib::singleton().p_PQexecParams(m_con, string("DECLARE BrigCursor BINARY NO SCROLL CURSOR FOR " + sql).c_str(), int(params.size()), types.data(), values.data(), lengths.data(), formats.data(), 1));
    m_res = lib::singleton().p_PQexec(m_con, string("FETCH FORWARD " + string_cast<char>(PageSize) + " FROM BrigCursor").c_str());
    check(PGRES_TUPLES_OK == lib::singleton().p_PQresultStatus(m_res));
  }
  else
  {
    m_res = lib::singleton().p_PQexecParams(m_con, sql.c_str(), int(params.size()), types.data(), values.data(), lengths.data(), formats.data(), 1);
    const ExecStatusType r(lib::singleton().p_PQresultStatus(m_res));
    check(r == PGRES_COMMAND_OK || r == PGRES_TUPLES_OK);
  }
}

inline void command::exec_batch(const std::string& sql)
{
  close_result();
  m_res = lib::singleton().p_PQexec(m_con, sql.c_str());
  const ExecStatusType r(lib::singleton().p_PQresultStatus(m_res));
  check(r == PGRES_COMMAND_OK || r == PGRES_TUPLES_OK);
  close_result();
}

inline std::vector<std::string> command::columns()
{
  std::vector<std::string> cols;
  if (!m_res) return cols;
  m_cols.clear();
  for (int i(0), count(lib::singleton().p_PQnfields(m_res)); i < count; ++i)
  {
    m_cols.push_back(get_value_factory(lib::singleton().p_PQftype(m_res, i)));
    cols.push_back(lib::singleton().p_PQfname(m_res, i));
  }
  return cols;
}

inline bool command::fetch(std::vector<variant>& row)
{
  if (!m_res) return false;

  if (m_fetch && m_row >= lib::singleton().p_PQntuples(m_res))
  {
    m_row = 0;
    lib::singleton().p_PQclear(m_res);
    m_res = lib::singleton().p_PQexec(m_con, std::string("FETCH FORWARD " + string_cast<char>(PageSize) + " FROM BrigCursor").c_str());
    check(PGRES_TUPLES_OK == lib::singleton().p_PQresultStatus(m_res));
  }

  if (m_row >= lib::singleton().p_PQntuples(m_res))
  {
    close_result();
    return false;
  }
  
  if (m_cols.empty()) columns();
  row.resize(m_cols.size());
  const int i(m_row++);
  for (size_t j(0); j < m_cols.size(); ++j)
  {
    if (lib::singleton().p_PQgetisnull(m_res, i, int(j))) row[j] = null_t();
    else m_cols[j](m_res, i, int(j), row[j]);
  }
  return true;
}

inline void command::set_autocommit(bool autocommit)
{
  close_result();
  if (m_autocommit == autocommit) return;
  check_command(lib::singleton().p_PQexec(m_con, autocommit? "ROLLBACK": "BEGIN"));
  m_autocommit = autocommit;
}

inline void command::commit()
{
  close_result();
  if (m_autocommit) return;
  check_command(lib::singleton().p_PQexec(m_con, "COMMIT; BEGIN;"));
} // command::

} } } } // brig::database::postgres::detail

#endif // BRIG_DATABASE_POSTGRES_DETAIL_COMMAND_HPP
