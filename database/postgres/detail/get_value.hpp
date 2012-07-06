// Andrew Naplavkov

#ifndef BRIG_DATABASE_POSTGRES_DETAIL_GET_VALUE_HPP
#define BRIG_DATABASE_POSTGRES_DETAIL_GET_VALUE_HPP

#include <boost/utility.hpp>
#include <brig/database/postgres/detail/lib.hpp>
#include <brig/database/variant.hpp>

namespace brig { namespace database { namespace postgres { namespace detail {

struct get_value : ::boost::noncopyable {
  virtual ~get_value()  {}
  virtual void operator()(PGresult* res, int row, int col, variant& var) = 0;
}; // get_value

} } } } // brig::database::postgres::detail

#endif // BRIG_DATABASE_POSTGRES_DETAIL_GET_VALUE_HPP
