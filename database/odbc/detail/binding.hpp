// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_DETAIL_BINDING_HPP
#define BRIG_DATABASE_ODBC_DETAIL_BINDING_HPP

#include <boost/utility.hpp>
#include <brig/database/odbc/detail/lib.hpp>

namespace brig { namespace database { namespace odbc { namespace detail {

struct binding : ::boost::noncopyable {
  virtual ~binding()  {}
  virtual SQLSMALLINT c_type() = 0;
  virtual SQLSMALLINT sql_type() = 0;
  virtual SQLULEN precision() = 0;
  virtual SQLPOINTER val_ptr() = 0;
  virtual SQLLEN* ind() = 0;
}; // binding

} } } } // brig::database::odbc::detail

#endif // BRIG_DATABASE_ODBC_DETAIL_BINDING_HPP
