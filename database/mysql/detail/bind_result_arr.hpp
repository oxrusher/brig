// Andrew Naplavkov

#ifndef BRIG_DATABASE_MYSQL_DETAIL_BIND_RESULT_ARR_HPP
#define BRIG_DATABASE_MYSQL_DETAIL_BIND_RESULT_ARR_HPP

#include <brig/database/mysql/detail/bind_result.hpp>
#include <brig/database/mysql/detail/lib.hpp>
#include <brig/database/null_t.hpp>
#include <brig/database/variant.hpp>
#include <cstring>

namespace brig { namespace database { namespace mysql { namespace detail {

template <typename T, enum_field_types TypeID>
class bind_result_arr : public bind_result {
  T m_arr;
  unsigned long m_lenght;
public:
  explicit bind_result_arr(MYSQL_BIND& bind);
  virtual int operator()(MYSQL_STMT* stmt, unsigned int col, variant& var);
}; // bind_result_arr

template <typename T, enum_field_types TypeID>
bind_result_arr<T, TypeID>::bind_result_arr(MYSQL_BIND& bind) : bind_result(bind), m_lenght(0)
{
  m_bind.buffer_type = TypeID;
  m_bind.length = &m_lenght;
}

template <typename T, enum_field_types TypeID>
int bind_result_arr<T, TypeID>::operator()(MYSQL_STMT* stmt, unsigned int col, variant& var)
{
  int r(0);
  if (m_is_null) var = null_t();
  else
  {
    if (m_bind.buffer_length < m_lenght) m_arr.resize(m_lenght);
    m_bind.buffer = (void*)m_arr.data();
    m_bind.buffer_length = m_lenght;
    r = lib::singleton().p_mysql_stmt_fetch_column(stmt, &m_bind, col, 0);
    m_bind.buffer = 0;
    m_bind.buffer_length = 0;

    if (r == 0)
    {
      var = T();
      T& arr = ::boost::get<T>(var);
      arr.resize(size_t(m_lenght));
      if (!arr.empty()) memcpy((void*)arr.data(), m_arr.data(), arr.size());
    }
  }
  return r;
} // bind_result_arr::

} } } } // brig::database::mysql::detail

#endif // BRIG_DATABASE_MYSQL_DETAIL_BIND_RESULT_ARR_HPP
