// Andrew Naplavkov

#ifndef BRIG_DATABASE_POSTGRES_DETAIL_BINDING_HPP
#define BRIG_DATABASE_POSTGRES_DETAIL_BINDING_HPP

#include <boost/utility.hpp>
#include <brig/database/postgres/detail/lib.hpp>

namespace brig { namespace database { namespace postgres { namespace detail {

struct binding : ::boost::noncopyable {
  virtual ~binding()  {}
  virtual Oid type() = 0;
  virtual const char* value() = 0;
  virtual int length() = 0;
  virtual int format() = 0;
}; // binding

} } } } // brig::database::postgres::detail

#endif // BRIG_DATABASE_POSTGRES_DETAIL_BINDING_HPP
