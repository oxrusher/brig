// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_DETAIL_DEFINE_HPP
#define BRIG_DATABASE_ORACLE_DETAIL_DEFINE_HPP

#include <boost/utility.hpp>
#include <brig/database/variant.hpp>

namespace brig { namespace database { namespace oracle { namespace detail {

struct define : boost::noncopyable {
  virtual ~define()  {}
  virtual void operator()(variant& var) = 0;
}; // define

} } } } // brig::database::oracle::detail

#endif // BRIG_DATABASE_ORACLE_DETAIL_DEFINE_HPP
