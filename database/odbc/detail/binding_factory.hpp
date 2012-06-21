// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_DETAIL_BINDING_FACTORY_HPP
#define BRIG_DATABASE_ODBC_DETAIL_BINDING_FACTORY_HPP

#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <brig/blob_t.hpp>
#include <brig/database/column_definition.hpp>
#include <brig/database/global.hpp>
#include <brig/database/odbc/detail/binding.hpp>
#include <brig/database/odbc/detail/binding_blob.hpp>
#include <brig/database/odbc/detail/binding_impl.hpp>
#include <brig/database/odbc/detail/binding_null.hpp>
#include <brig/database/odbc/detail/binding_string.hpp>
#include <brig/database/odbc/detail/lib.hpp>
#include <brig/database/null_t.hpp>
#include <brig/database/variant.hpp>
#include <cstdint>
#include <string>

namespace brig { namespace database { namespace odbc { namespace detail {

class binding_visitor : public ::boost::static_visitor<binding*> {
  const DBMS m_sys;
  const column_definition* m_col;
public:
  explicit binding_visitor(DBMS sys_, const column_definition* col_) : m_sys(sys_), m_col(col_)  {}
  binding* operator()(const null_t&) const  { return new binding_null(m_col? m_col->type: VoidColumn, m_sys); }
  binding* operator()(int16_t v) const  { return new binding_impl<int16_t, SQL_C_SSHORT, SQL_SMALLINT>(v); }
  binding* operator()(int32_t v) const  { return new binding_impl<int32_t, SQL_C_SLONG, SQL_INTEGER>(v); }
  binding* operator()(int64_t v) const  { return Postgres == m_sys? operator()(int32_t(v)): new binding_impl<int64_t, SQL_C_SBIGINT, SQL_BIGINT>(v); } // todo:
  binding* operator()(float v) const  { return new binding_impl<float, SQL_C_FLOAT, SQL_REAL>(v); }
  binding* operator()(double v) const  { return new binding_impl<double, SQL_C_DOUBLE, SQL_DOUBLE>(v); }
  binding* operator()(const std::string& r) const  { return new binding_string(r, m_sys); }
  binding* operator()(const blob_t& r) const  { return new binding_blob(r, m_sys); }
}; // binding_visitor

inline binding* binding_factory(DBMS sys, const variant& var, const column_definition* param_col)
{
  return ::boost::apply_visitor(binding_visitor(sys, param_col), var);
}

} } } } // brig::database::odbc::detail

#endif // BRIG_DATABASE_ODBC_DETAIL_BINDING_FACTORY_HPP
