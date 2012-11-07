// Andrew Naplavkov

#ifndef BRIG_DATABASE_POSTGRES_DETAIL_COMMAND_HPP
#define BRIG_DATABASE_POSTGRES_DETAIL_COMMAND_HPP

#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <boost/ptr_container/ptr_vector.hpp>
#include <brig/database/command.hpp>
#include <brig/database/global.hpp>
#include <brig/database/identifier.hpp>
#include <brig/database/postgres/detail/binding_factory.hpp>
#include <brig/database/postgres/detail/get_value_factory.hpp>
#include <brig/database/postgres/detail/lib.hpp>
#include <brig/string_cast.hpp>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>

namespace brig { namespace database { namespace postgres { namespace detail {

class command : public brig::database::command {
  PGconn* m_con;
  PGresult* m_res;
  ::boost::ptr_vector<get_value> m_cols;
  int m_row;
  bool m_autocommit;

  void check(bool r);
  void close_result();
  void close_all();

public:
  command(const std::string& host, int port, const std::string& db, const std::string& usr, const std::string& pwd);
  virtual ~command()  { close_all(); }
  virtual void exec(const std::string& sql, const std::vector<column_definition>& params = std::vector<column_definition>());
  virtual size_t affected();
  virtual std::vector<std::string> columns();
  virtual bool fetch(std::vector<variant>& row);
  virtual void set_autocommit(bool autocommit);
  virtual void commit();
  virtual DBMS system()  { return Postgres; }
  virtual command_traits traits();
}; // command

inline void command::check(bool r)
{
  if (r) return;
  std::string msg(lib::singleton().p_PQerrorMessage(m_con));
  if (msg.empty()) msg = "Postgres error";
  throw std::runtime_error(msg);
}

inline void command::close_result()
{
  if (!m_res) return;
  m_row = 0;
  m_cols.clear();
  PGresult* res(0); std::swap(res, m_res);
  lib::singleton().p_PQclear(res);
}

inline void command::close_all()
{
  close_result();
  lib::singleton().p_PQfinish(m_con);
}

inline command::command(const std::string& host, int port, const std::string& db, const std::string& usr, const std::string& pwd)
  : m_con(0), m_res(0), m_row(0), m_autocommit(true)
{
  if (lib::singleton().empty()) throw std::runtime_error("Postgres error");
  m_con = lib::singleton().p_PQsetdbLogin((char*)host.c_str(), (char*)string_cast<char>(port).c_str(), 0, 0, (char*)db.c_str(), (char*)usr.c_str(), (char*)pwd.c_str());
  check(lib::singleton().p_PQstatus(m_con) == CONNECTION_OK);
  try  { check(lib::singleton().p_PQsetClientEncoding(m_con, "UTF8") == 0); }
  catch (const std::exception&)  { close_all(); throw; }
}

inline void command::exec(const std::string& sql, const std::vector<column_definition>& params)
{
  close_result();
  ::boost::ptr_vector<binding> binds;
  std::vector<Oid> types;
  std::vector<char*> values;
  std::vector<int> lengths;
  std::vector<int> formats;
  for (size_t i(0); i < params.size(); ++i)
  {
    binding* bind(binding_factory(params[i].query_value));
    binds.push_back(bind);
    types.push_back(bind->type());
    values.push_back((char*)bind->value());
    lengths.push_back(bind->length());
    formats.push_back(bind->format());
  }
  m_res = lib::singleton().p_PQexecParams(m_con, sql.c_str(), int(params.size()), types.data(), values.data(), lengths.data(), formats.data(), 1);
  const ExecStatusType r(lib::singleton().p_PQresultStatus(m_res));
  check(r == PGRES_COMMAND_OK || r == PGRES_TUPLES_OK);
}

inline size_t command::affected()
{
  if (!m_res) return 0;
  try  { return ::boost::lexical_cast<size_t>(lib::singleton().p_PQcmdTuples(m_res)); }
  catch (::boost::bad_lexical_cast&)  { return 0; }
}

inline std::vector<std::string> command::columns()
{
  std::vector<std::string> cols;
  if (!m_res) return cols;
  m_cols.clear();
  int count(lib::singleton().p_PQnfields(m_res));
  for (int i(0); i < count; ++i)
  {
    m_cols.push_back(get_value_factory(lib::singleton().p_PQftype(m_res, i)));
    cols.push_back(lib::singleton().p_PQfname(m_res, i));
  }
  return cols;
}

inline bool command::fetch(std::vector<variant>& row)
{
  if (!m_res || m_row >= lib::singleton().p_PQntuples(m_res)) return false;
  if (m_cols.empty()) columns();
  row.resize(m_cols.size());
  const int i(m_row); ++m_row;
  for (size_t j(0); j < m_cols.size(); ++j)
  {
    if (lib::singleton().p_PQgetisnull(m_res, i, j)) row[j] = null_t();
    else m_cols[j](m_res, i, j, row[j]);
  }
  return true;
}

inline void command::set_autocommit(bool autocommit)
{
  if (m_autocommit == autocommit) return;
  exec(autocommit? "ROLLBACK": "BEGIN");
  m_autocommit = autocommit;
}

inline void command::commit()
{
  if (m_autocommit) return;
  exec("COMMIT");
  exec("BEGIN");
}

inline command_traits command::traits()
{
  command_traits trs;
  trs.parameter_prefix = '$';
  trs.parameter_suffix = true;
  return trs;
} // command::

} } } } // brig::database::postgres::detail

#endif // BRIG_DATABASE_POSTGRES_DETAIL_COMMAND_HPP
