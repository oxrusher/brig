// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_DETAIL_GET_DATA_HPP
#define BRIG_DATABASE_ODBC_DETAIL_GET_DATA_HPP

#include <boost/utility.hpp>
#include <brig/database/odbc/detail/lib.hpp>
#include <brig/database/variant.hpp>

namespace brig { namespace database { namespace odbc { namespace detail {

struct get_data : boost::noncopyable {
  virtual ~get_data()  {}
  virtual SQLRETURN operator()(SQLHSTMT stmt, size_t col, variant& var) = 0;
}; // get_data

} } } } // brig::database::odbc::detail

#endif // BRIG_DATABASE_ODBC_DETAIL_GET_DATA_HPP
