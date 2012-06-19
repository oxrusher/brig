// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_DETAIL_COMMAND_HPP
#define BRIG_DATABASE_ORACLE_DETAIL_COMMAND_HPP

#include <boost/ptr_container/ptr_vector.hpp>
#include <brig/database/command.hpp>
#include <brig/database/detail/sql_identifier.hpp>
#include <brig/database/global.hpp>
#include <brig/database/identifier.hpp>
#include <brig/database/oracle/detail/binding.hpp>
#include <brig/database/oracle/detail/binding_factory.hpp>
#include <brig/database/oracle/detail/define.hpp>
#include <brig/database/oracle/detail/define_factory.hpp>
#include <brig/database/oracle/detail/handles.hpp>
#include <brig/database/oracle/detail/lib.hpp>
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

  void close_all();
  void close_stmt();

public:
  command(const std::string& srv, const std::string& usr, const std::string& pwd);
  virtual ~command()  { close_all(); }
  virtual DBMS system()  { return Oracle; }
  virtual std::string sql_parameter(size_t order, const column_definition& param_col);
  virtual std::string sql_column(const column_definition& col);
  virtual void exec
    ( const std::string& sql
    , const std::vector<variant>& params = std::vector<variant>()
    , const std::vector<column_definition>& param_cols = std::vector<column_definition>()
    );
  virtual size_t affected();
  virtual std::vector<std::string> columns();
  virtual bool fetch(std::vector<variant>& row);
  virtual void set_autocommit(bool autocommit);
  virtual void commit();
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
  using namespace brig::unicode;

  const std::u16string
    srv(transform<std::u16string>(srv_)),
    usr(transform<std::u16string>(usr_)),
    pwd(transform<std::u16string>(pwd_)),
    type_schema(transform<std::u16string>("MDSYS")),
    type_name(transform<std::u16string>("SDO_GEOMETRY"));

  try
  {
    if (lib::singleton().empty()) throw std::runtime_error("OCI error");
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
  catch (const std::exception&)  { close_all(); throw; }
}

inline void command::exec(const std::string& sql_, const std::vector<variant>& params, const std::vector<column_definition>& param_cols)
{
  const std::u16string sql(brig::unicode::transform<std::u16string>(sql_));

  close_stmt();
  m_hnd.alloc_handle((void**)&m_hnd.stmt, OCI_HTYPE_STMT);
  m_hnd.check(lib::singleton().p_OCIStmtPrepare(m_hnd.stmt, m_hnd.err, (const text*)sql.c_str(), ub4(sql.size() * sizeof(char16_t)), OCI_NTV_SYNTAX, OCI_DEFAULT));
  ub2 stmt_type(0);
  m_hnd.check(lib::singleton().p_OCIAttrGet(m_hnd.stmt, OCI_HTYPE_STMT, &stmt_type, 0, OCI_ATTR_STMT_TYPE, m_hnd.err));

  ::boost::ptr_vector<binding> binds;
  for (size_t i(0); i < params.size(); ++i)
    binds.push_back(binding_factory(&m_hnd, i, params[i], i < param_cols.size()? &param_cols[i]: 0));

  m_hnd.check(lib::singleton().p_OCIStmtExecute(m_hnd.svc, m_hnd.stmt, m_hnd.err, OCI_STMT_SELECT == stmt_type? 0: 1, 0, 0, 0, m_autocommit? OCI_COMMIT_ON_SUCCESS: OCI_DEFAULT));
}

inline size_t command::affected()
{
  ub4 count(0);
  if (0 != m_hnd.stmt) m_hnd.check(lib::singleton().p_OCIAttrGet(m_hnd.stmt, OCI_HTYPE_STMT, &count, 0, OCI_ATTR_ROW_COUNT, m_hnd.err));
  return count;
}

inline std::vector<std::string> command::columns()
{
  using namespace brig::unicode;

  std::vector<std::string> cols;
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

      identifier dbms_type_lcase;
      dbms_type_lcase.schema = transform<std::string>(type_schema, lower_case);
      dbms_type_lcase.name = transform<std::string>(type_name, lower_case);

      m_cols.push_back(define_factory(&m_hnd, i + 1, data_type, size, precision, scale, dbms_type_lcase));
      cols.push_back(transform<std::string>(name));

    }
    catch (const std::exception&)  { handles::free_descriptor((void**)&dsc, OCI_DTYPE_PARAM); throw; }
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

inline std::string command::sql_parameter(size_t order, const column_definition& param_col)
{
  using namespace brig::database::detail;
  std::ostringstream stream; stream.imbue(std::locale::classic());
  if (Geometry == param_col.type && "sdo_geometry" != param_col.dbms_type_lcase.name)
    stream << sql_identifier(Oracle, param_col.dbms_type) << "(:" << (order + 1) << ")";
  else
    stream << ":" << (order + 1);
  return stream.str();
}

inline std::string command::sql_column(const column_definition& col)
{
  using namespace brig::database::detail;
  if (col.sql_expression.empty() && Geometry == col.type)
  {
    const std::string id(sql_identifier(Oracle, col.name));
    if ("sdo_geometry" == col.dbms_type_lcase.name) return id;
    else return sql_identifier(Oracle, col.dbms_type) + ".GET_SDO_GEOM(" + id + ") as " + id;
  }
  return brig::database::command::sql_column(col);
}

inline void command::set_autocommit(bool autocommit)
{
  if (m_autocommit == autocommit) return;
  close_stmt();
  if (autocommit) m_hnd.check(lib::singleton().p_OCITransRollback(m_hnd.svc, m_hnd.err, OCI_DEFAULT));
  m_autocommit = autocommit;
}

inline void command::commit()
{
  if (m_autocommit) return;
  close_stmt();
  m_hnd.check(lib::singleton().p_OCITransCommit(m_hnd.svc, m_hnd.err, OCI_DEFAULT));
} // command::

} } } } // brig::database::oracle::detail

#endif // BRIG_DATABASE_ORACLE_DETAIL_COMMAND_HPP
