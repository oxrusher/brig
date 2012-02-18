// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_DETAIL_BINDING_HPP
#define BRIG_DATABASE_ORACLE_DETAIL_BINDING_HPP

#include <boost/utility.hpp>

namespace brig { namespace database { namespace oracle { namespace detail {

struct binding : ::boost::noncopyable {
  virtual ~binding()  {}
}; // binding

} } } } // brig::database::oracle::detail

#endif // BRIG_DATABASE_ORACLE_DETAIL_BINDING_HPP
