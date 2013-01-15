// Andrew Naplavkov

#ifndef BRIG_DATABASE_MYSQL_DETAIL_BIND_RESULT_HPP
#define BRIG_DATABASE_MYSQL_DETAIL_BIND_RESULT_HPP

#include <boost/utility.hpp>
#include <brig/database/mysql/detail/lib.hpp>
#include <brig/variant.hpp>

namespace brig { namespace database { namespace mysql { namespace detail {

class bind_result : public ::boost::noncopyable {
protected:
  MYSQL_BIND& m_bind;
  my_bool m_is_null;
public:
  explicit bind_result(MYSQL_BIND& bind) : m_bind(bind), m_is_null(0)  { m_bind.is_null = &m_is_null; }
  virtual ~bind_result()  {}
  virtual int operator()(MYSQL_STMT* stmt, unsigned int col, variant& var) = 0;
}; // bind_result

} } } } // brig::database::mysql::detail

#endif // BRIG_DATABASE_MYSQL_DETAIL_BIND_RESULT_HPP
