// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_DETAIL_BINDING_FACTORY_HPP
#define BRIG_DATABASE_ODBC_DETAIL_BINDING_FACTORY_HPP

#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <brig/blob_t.hpp>
#include <brig/database/column_detail.hpp>
#include <brig/database/detail/get_type.hpp>
#include <brig/database/global.hpp>
#include <brig/database/odbc/detail/binding.hpp>
#include <brig/database/odbc/detail/binding_blob.hpp>
#include <brig/database/odbc/detail/binding_impl.hpp>
#include <brig/database/odbc/detail/binding_null.hpp>
#include <brig/database/odbc/detail/binding_string.hpp>
#include <brig/database/odbc/detail/lib.hpp>
#include <brig/database/variant.hpp>
#include <cstdint>

namespace brig { namespace database { namespace odbc { namespace detail {

struct binding_visitor : boost::static_visitor<binding*> {
  const DBMS sys;
  const column_detail* col;
  explicit binding_visitor(DBMS sys_, const column_detail* col_) : sys(sys_), col(col_)  {}
  binding* operator()(const null_t&) const  { return new binding_null(col? brig::database::detail::get_type(sys, *col): VoidColumn); }
  binding* operator()(int16_t v) const  { return new binding_impl<int16_t, SQL_C_SSHORT, SQL_SMALLINT>(v); }
  binding* operator()(int32_t v) const  { return new binding_impl<int32_t, SQL_C_SLONG, SQL_INTEGER>(v); }
  binding* operator()(int64_t v) const  { return Postgres == sys? operator()(int32_t(v)): new binding_impl<int64_t, SQL_C_SBIGINT, SQL_BIGINT>(v); }
  binding* operator()(float v) const  { return new binding_impl<float, SQL_C_FLOAT, SQL_REAL>(v); }
  binding* operator()(double v) const  { return new binding_impl<double, SQL_C_DOUBLE, SQL_DOUBLE>(v); }
  binding* operator()(const boost::gregorian::date&) const;
  binding* operator()(const boost::posix_time::ptime&) const;
  binding* operator()(const std::string& r) const  { return new binding_string(r, MS_SQL == sys? SQL_WLONGVARCHAR: SQL_WVARCHAR); }
  binding* operator()(const blob_t& r) const  { return new binding_blob(r, MS_SQL == sys? SQL_LONGVARBINARY: SQL_VARBINARY); }
}; // binding_visitor

inline binding* binding_visitor::operator()(const boost::gregorian::date& r) const
{
  DATE_STRUCT v;
  v.year = SQLSMALLINT(r.year());
  v.month = SQLUSMALLINT(r.month());
  v.day = SQLUSMALLINT(r.day());
  return new binding_impl<DATE_STRUCT, SQL_C_TYPE_DATE, SQL_DATE>(v);
}

inline binding* binding_visitor::operator()(const boost::posix_time::ptime& r) const
{
  TIMESTAMP_STRUCT v;
  const boost::gregorian::date d(r.date());
  v.year = SQLSMALLINT(d.year());
  v.month = SQLUSMALLINT(d.month());
  v.day = SQLUSMALLINT(d.day());
  const boost::posix_time::time_duration t(r.time_of_day());
  v.hour = SQLUSMALLINT(t.hours());
  v.minute = SQLUSMALLINT(t.minutes());
  v.second = SQLUSMALLINT(t.seconds());
  v.fraction = SQLUINTEGER(0);
  return new binding_impl<TIMESTAMP_STRUCT, SQL_C_TYPE_TIMESTAMP, SQL_TIMESTAMP>(v);
} // binding_visitor::

inline binding* binding_factory(DBMS sys, const brig::database::variant& var, const column_detail* param_col)
{
  return boost::apply_visitor(binding_visitor(sys, param_col), var);
}

} } } } // brig::database::odbc::detail

#endif // BRIG_DATABASE_ODBC_DETAIL_BINDING_FACTORY_HPP
