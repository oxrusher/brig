// Andrew Naplavkov

#ifndef BRIG_DATABASE_MYSQL_DETAIL_BIND_PARAM_HPP
#define BRIG_DATABASE_MYSQL_DETAIL_BIND_PARAM_HPP

#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <brig/blob_t.hpp>
#include <brig/database/mysql/detail/lib.hpp>
#include <brig/database/null_t.hpp>
#include <brig/database/variant.hpp>
#include <cstdint>
#include <string>

namespace brig { namespace database { namespace mysql { namespace detail {

struct bind_param_visitor : ::boost::static_visitor<void> {
  MYSQL_BIND& b;
  explicit bind_param_visitor(MYSQL_BIND& b_) : b(b_)  {}
  void operator()(const null_t&) const  { b.buffer_type = MYSQL_TYPE_NULL; }
  void operator()(const int16_t& r) const  { b.buffer_type = MYSQL_TYPE_SHORT; b.buffer = (char*)&r; }
  void operator()(const int32_t& r) const  { b.buffer_type = MYSQL_TYPE_LONG; b.buffer = (char*)&r; }
  void operator()(const int64_t& r) const  { b.buffer_type = MYSQL_TYPE_LONGLONG; b.buffer = (char*)&r; }
  void operator()(const float& r) const  { b.buffer_type = MYSQL_TYPE_FLOAT; b.buffer = (char*)&r; }
  void operator()(const double& r) const  { b.buffer_type = MYSQL_TYPE_DOUBLE; b.buffer = (char*)&r; }
  void operator()(const std::string& r) const  { b.buffer_type = MYSQL_TYPE_STRING; b.buffer = (char*)r.c_str(); b.buffer_length = (unsigned long)r.size(); }
  void operator()(const blob_t& r) const  { b.buffer_type = MYSQL_TYPE_BLOB; b.buffer = (char*)r.data(); b.buffer_length = (unsigned long)r.size(); }
}; // bind_param_visitor

inline void bind_param(const variant& param, MYSQL_BIND& bind)
{
  bind_param_visitor visitor(bind);
  return ::boost::apply_visitor(visitor, param);
}

} } } } // brig::database::mysql::detail

#endif // BRIG_DATABASE_MYSQL_DETAIL_BIND_PARAM_HPP
