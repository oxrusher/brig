// Andrew Naplavkov

#ifndef BRIG_DATABASE_SQLITE_DETAIL_BINDING_HPP
#define BRIG_DATABASE_SQLITE_DETAIL_BINDING_HPP

#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/static_visitor.hpp>
#include <brig/blob_t.hpp>
#include <brig/database/null_t.hpp>
#include <brig/database/sqlite/detail/lib.hpp>
#include <brig/database/variant.hpp>
#include <cstdint>
#include <string>

namespace brig { namespace database { namespace sqlite { namespace detail {

struct binding_visitor : ::boost::static_visitor<int> {
  sqlite3_stmt* stmt;
  int i;

  int operator()(const null_t&) const  { return lib::singleton().p_sqlite3_bind_null(stmt, i); }
  template <typename T>
  int operator()(T v) const  { return lib::singleton().p_sqlite3_bind_int64(stmt, i, int64_t(v)); }
  int operator()(float v) const  { return lib::singleton().p_sqlite3_bind_double(stmt, i, double(v)); }
  int operator()(double v) const  { return lib::singleton().p_sqlite3_bind_double(stmt, i, v); }
  int operator()(const blob_t& r) const  { return lib::singleton().p_sqlite3_bind_blob(stmt, i, r.data(), int(r.size()), SQLITE_STATIC); }
  int operator()(const std::string& r) const  { return lib::singleton().p_sqlite3_bind_text(stmt, i, r.c_str(), -1, SQLITE_STATIC); }
}; // binding_visitor

inline int bind(sqlite3_stmt* stmt, size_t order, const variant& param)
{
  binding_visitor visitor;
  visitor.stmt = stmt;
  visitor.i = int(order + 1);
  return ::boost::apply_visitor(visitor, param);
}

} } } } // brig::database::sqlite::detail

#endif // BRIG_DATABASE_SQLITE_DETAIL_BINDING_HPP
