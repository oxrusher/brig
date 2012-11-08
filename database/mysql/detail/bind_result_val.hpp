// Andrew Naplavkov

#ifndef BRIG_DATABASE_MYSQL_DETAIL_BIND_RESULT_VAL_HPP
#define BRIG_DATABASE_MYSQL_DETAIL_BIND_RESULT_VAL_HPP

#include <brig/database/mysql/detail/bind_result.hpp>
#include <brig/database/mysql/detail/lib.hpp>
#include <brig/database/null_t.hpp>
#include <brig/database/variant.hpp>

namespace brig { namespace database { namespace mysql { namespace detail {

template <typename T, enum_field_types TypeID>
class bind_result_val : public bind_result {
  T m_val;
public:
  explicit bind_result_val(MYSQL_BIND& bind);
  int operator()(MYSQL_STMT* stmt, unsigned int col, variant& var) override;
}; // bind_result_val

template <typename T, enum_field_types TypeID>
bind_result_val<T, TypeID>::bind_result_val(MYSQL_BIND& bind) : bind_result(bind), m_val(0)
{
  m_bind.buffer_type = TypeID;
  m_bind.buffer = (void*)&m_val;
  m_bind.buffer_length = sizeof(T);
}

template <typename T, enum_field_types TypeID>
int bind_result_val<T, TypeID>::operator()(MYSQL_STMT*, unsigned int, variant& var)
{
  if (m_is_null) var = null_t();
  else var = m_val;
  return 0;
} // bind_result_val::

} } } } // brig::database::mysql::detail

#endif // BRIG_DATABASE_MYSQL_DETAIL_BIND_RESULT_VAL_HPP
