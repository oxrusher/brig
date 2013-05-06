// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_DETAIL_COMMAND_HPP
#define BRIG_DATABASE_ORACLE_DETAIL_COMMAND_HPP

#include <boost/ptr_container/ptr_vector.hpp>
#include <brig/database/command.hpp>
#include <brig/database/oracle/detail/binding.hpp>
#include <brig/database/oracle/detail/binding_factory.hpp>
#include <brig/database/oracle/detail/define.hpp>
#include <brig/database/oracle/detail/define_factory.hpp>
#include <brig/database/oracle/detail/handles.hpp>
#include <brig/database/oracle/detail/lib.hpp>
#include <brig/global.hpp>
#include <brig/identifier.hpp>
#include <brig/string_cast.hpp>
#include <brig/unicode/lower_case.hpp>
#include <brig/unicode/transform.hpp>
#include <locale>
#include <sstream>
#include <stdexcept>
#include <string>

namespace brig { namespace database { namespace oracle { namespace detail {

class command : public brig::database::command {
  handles m_hnd;
  ::boost::ptr_vector<define> m_cols;
  bool m_autocommit;

  void close_stmt();
  void close_all();

public:
  command(const std::string& srv, const std::string& usr, const std::string& pwd);
  ~command() override  { close_all(); }
  void exec(const std::string& sql, const std::vector<column_def>& params = std::vector<column_def>()) override;
  void exec_batch(const std::string& sql) override;
  std::vector<std::string> columns() override;
  bool fetch(std::vector<variant>& row) override;
  void set_autocommit(bool autocommit) override;
  void commit() override;
  DBMS system() override  { return DBMS::Oracle; }
  std::string sql_param(size_t order) override  { return ":" + string_cast<char>(order + 1); }
  bool readable_geom() override { return true; }
  bool writable_geom() override { return true; }
}; // command

inline void command::close_stmt()
{
  m_cols.clear();
  handles::free_handle((void**)&m_hnd.stmt, OCI_HTYPE_STMT);
}

inline void command::close_all()
{
  m_autocommit = true;
  close_stmt();
  m_hnd.geom = 0;
  handles::free_handle((void**)&m_hnd.ses, OCI_HTYPE_SESSION);
  handles::free_handle((void**)&m_hnd.svc, OCI_HTYPE_SVCCTX);
  handles::free_handle((void**)&m_hnd.srv, OCI_HTYPE_SERVER);
  handles::free_handle((void**)&m_hnd.err, OCI_HTYPE_ERROR);
  handles::free_handle((void**)&m_hnd.env, OCI_HTYPE_ENV);
}

inline command::command(const std::string& srv_, const std::string& usr_, const std::string& pwd_) : m_autocommit(true)
{
  using namespace std;
  using namespace brig::unicode;

  const u16string
    srv(transform<char16_t>(srv_)),
    usr(transform<char16_t>(usr_)),
    pwd(transform<char16_t>(pwd_)),
    type_schema(transform<char16_t>("MDSYS")),
    type_name(transform<char16_t>("SDO_GEOMETRY"));

  try
  {
    if (lib::singleton().empty()) throw runtime_error("OCI error");
    OCIEnv* env(0);
    m_hnd.check(lib::singleton().p_OCIEnvNlsCreate(&env, OCI_THREADED|OCI_NO_MUTEX|OCI_OBJECT, 0, 0, 0, 0, 0, 0, OCI_UTF16ID, OCI_UTF16ID));
    m_hnd.env = env;
    m_hnd.alloc_handle((void**)&m_hnd.err, OCI_HTYPE_ERROR);
    m_hnd.alloc_handle((void**)&m_hnd.srv, OCI_HTYPE_SERVER);
    m_hnd.alloc_handle((void**)&m_hnd.svc, OCI_HTYPE_SVCCTX);
    m_hnd.alloc_handle((void**)&m_hnd.ses, OCI_HTYPE_SESSION);
    m_hnd.check(lib::singleton().p_OCIServerAttach(m_hnd.srv, m_hnd.err, (const text*)srv.c_str(), ub4(srv.size() * sizeof(char16_t)), OCI_DEFAULT));
    m_hnd.check(lib::singleton().p_OCIAttrSet(m_hnd.svc, OCI_HTYPE_SVCCTX, (void*)m_hnd.srv, 0, OCI_ATTR_SERVER, m_hnd.err));
    m_hnd.check(lib::singleton().p_OCIAttrSet(m_hnd.ses, OCI_HTYPE_SESSION, (void*)usr.c_str(), ub4(usr.size() * sizeof(char16_t)), OCI_ATTR_USERNAME, m_hnd.err));
    m_hnd.check(lib::singleton().p_OCIAttrSet(m_hnd.ses, OCI_HTYPE_SESSION, (void*)pwd.c_str(), ub4(pwd.size() * sizeof(char16_t)), OCI_ATTR_PASSWORD, m_hnd.err));
    m_hnd.check(lib::singleton().p_OCISessionBegin(m_hnd.svc, m_hnd.err, m_hnd.ses, OCI_CRED_RDBMS, OCI_DEFAULT));
    m_hnd.check(lib::singleton().p_OCIAttrSet(m_hnd.svc, OCI_HTYPE_SVCCTX, (void*)m_hnd.ses, 0, OCI_ATTR_SESSION, m_hnd.err));
    OCIType* type(0);
    m_hnd.check(lib::singleton().p_OCITypeByName
      ( m_hnd.env, m_hnd.err, m_hnd.svc
      , (const text*)type_schema.c_str(), ub4(type_schema.size() * sizeof(char16_t))
      , (const text*)type_name.c_str(), ub4(type_name.size() * sizeof(char16_t))
      , (const text*)0, 0, OCI_DURATION_SESSION, OCI_TYPEGET_HEADER, &type));
    m_hnd.geom = type;
  }
  catch (const exception&)  { close_all(); throw; }
}

inline void command::exec(const std::string& sql_, const std::vector<column_def>& params)
{
  using namespace std;

  close_stmt();
  m_hnd.alloc_handle((void**)&m_hnd.stmt, OCI_HTYPE_STMT);
  const u16string sql(brig::unicode::transform<char16_t>(sql_));
  m_hnd.check(lib::singleton().p_OCIStmtPrepare(m_hnd.stmt, m_hnd.err, (const text*)sql.c_str(), ub4(sql.size() * sizeof(char16_t)), OCI_NTV_SYNTAX, OCI_DEFAULT));
  ub2 stmt_type(0);
  m_hnd.check(lib::singleton().p_OCIAttrGet(m_hnd.stmt, OCI_HTYPE_STMT, &stmt_type, 0, OCI_ATTR_STMT_TYPE, m_hnd.err));

  ::boost::ptr_vector<binding> binds;
  for (size_t i(0); i < params.size(); ++i)
    binds.push_back(binding_factory(&m_hnd, i, params[i]));

  m_hnd.check(lib::singleton().p_OCIStmtExecute(m_hnd.svc, m_hnd.stmt, m_hnd.err, OCI_STMT_SELECT == stmt_type? 0: 1, 0, 0, 0, m_autocommit? OCI_COMMIT_ON_SUCCESS: OCI_DEFAULT));
}

inline void command::exec_batch(const std::string& sql)
{
  exec(sql);
  close_stmt();
}

inline std::vector<std::string> command::columns()
{
  using namespace std;
  using namespace brig::unicode;

  vector<string> cols;
  if (0 == m_hnd.stmt) return cols;
  m_cols.clear();
  ub4 count(0);
  m_hnd.check(lib::singleton().p_OCIAttrGet(m_hnd.stmt, OCI_HTYPE_STMT, &count, 0, OCI_ATTR_PARAM_COUNT, m_hnd.err));
  for (ub4 i(0); i < count; ++i)
  {
    OCIParam *dsc(0);
    m_hnd.check(lib::singleton().p_OCIParamGet(m_hnd.stmt, OCI_HTYPE_STMT, m_hnd.err, (void**)&dsc, i + 1));
    try
    {

      ub2 data_type(0), size(0);
      sb2 precision(0);
      sb1 scale(-1);
      utext *name(0), *type_schema(0), *type_name(0);
      ub4 name_len(0), type_schema_len(0), type_name_len(0);

      m_hnd.check(lib::singleton().p_OCIAttrGet(dsc, OCI_DTYPE_PARAM, &name, &name_len, OCI_ATTR_NAME, m_hnd.err));
      m_hnd.check(lib::singleton().p_OCIAttrGet(dsc, OCI_DTYPE_PARAM, &data_type, 0, OCI_ATTR_DATA_TYPE, m_hnd.err));
      m_hnd.check(lib::singleton().p_OCIAttrGet(dsc, OCI_DTYPE_PARAM, &size, 0, OCI_ATTR_DATA_SIZE, m_hnd.err));
      m_hnd.check(lib::singleton().p_OCIAttrGet(dsc, OCI_DTYPE_PARAM, &precision, 0, OCI_ATTR_PRECISION, m_hnd.err));
      m_hnd.check(lib::singleton().p_OCIAttrGet(dsc, OCI_DTYPE_PARAM, &scale, 0, OCI_ATTR_SCALE, m_hnd.err));
      m_hnd.check(lib::singleton().p_OCIAttrGet(dsc, OCI_DTYPE_PARAM, &type_schema, &type_schema_len, OCI_ATTR_SCHEMA_NAME, m_hnd.err));
      m_hnd.check(lib::singleton().p_OCIAttrGet(dsc, OCI_DTYPE_PARAM, &type_name, &type_name_len, OCI_ATTR_TYPE_NAME, m_hnd.err));

      identifier type_lcase;
      type_lcase.schema = transform<char>(type_schema, lower_case);
      type_lcase.name = transform<char>(type_name, lower_case);

      m_cols.push_back(define_factory(&m_hnd, i + 1, data_type, size, precision, scale, type_lcase));
      cols.push_back(transform<char>(name));

    }
    catch (const exception&)  { handles::free_descriptor((void**)&dsc, OCI_DTYPE_PARAM); throw; }
    handles::free_descriptor((void**)&dsc, OCI_DTYPE_PARAM);
  }

  ub4 rows = ub4(PageSize);
  m_hnd.check(lib::singleton().p_OCIAttrSet(m_hnd.stmt, OCI_HTYPE_STMT, &rows, 0, OCI_ATTR_PREFETCH_ROWS, m_hnd.err));
  return cols;
}

inline bool command::fetch(std::vector<variant>& row)
{
  if (0 == m_hnd.stmt) return false;
  if (m_cols.empty()) columns();

  const sword r(lib::singleton().p_OCIStmtFetch2(m_hnd.stmt, m_hnd.err, 1, OCI_FETCH_NEXT, 1, OCI_DEFAULT));
  if (OCI_NO_DATA == r) return false;
  m_hnd.check(r);

  row.resize(m_cols.size());
  for (size_t i(0); i < m_cols.size(); ++i)
    m_cols[i](row[i]);
  return true;
}

inline void command::set_autocommit(bool autocommit)
{
  close_stmt();
  if (m_autocommit == autocommit) return;
  if (autocommit) m_hnd.check(lib::singleton().p_OCITransRollback(m_hnd.svc, m_hnd.err, OCI_DEFAULT));
  m_autocommit = autocommit;
}

inline void command::commit()
{
  close_stmt();
  if (m_autocommit) return;
  m_hnd.check(lib::singleton().p_OCITransCommit(m_hnd.svc, m_hnd.err, OCI_DEFAULT));
} // command::

} } } } // brig::database::oracle::detail

#endif // BRIG_DATABASE_ORACLE_DETAIL_COMMAND_HPP
