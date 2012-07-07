// Andrew Naplavkov

#ifndef BRIG_DATABASE_MYSQL_DETAIL_BIND_RESULT_HPP
#define BRIG_DATABASE_MYSQL_DETAIL_BIND_RESULT_HPP

#include <boost/utility.hpp>
#include <brig/database/mysql/detail/lib.hpp>
#include <brig/database/variant.hpp>

namespace brig { namespace database { namespace mysql { namespace detail {

class bind_result : public ::boost::noncopyable {
protected:
  my_bool m_is_null, m_error;
public:
  explicit bind_result(MYSQL_BIND& bind) : m_is_null(0), m_error(0)  { bind.is_null = &m_is_null; bind.error = &m_error; }
  virtual ~bind_result()  {}
  virtual void operator()(MYSQL_STMT* stmt, MYSQL_BIND& bind, unsigned int col, variant& var) = 0;
}; // bind_result

} } } } // brig::database::mysql::detail

#endif // BRIG_DATABASE_MYSQL_DETAIL_BIND_RESULT_HPP
