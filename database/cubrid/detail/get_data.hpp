// Andrew Naplavkov

#ifndef BRIG_DATABASE_CUBRID_DETAIL_GET_DATA_HPP
#define BRIG_DATABASE_CUBRID_DETAIL_GET_DATA_HPP

#include <boost/utility.hpp>
#include <brig/variant.hpp>

namespace brig { namespace database { namespace cubrid { namespace detail {

struct get_data : ::boost::noncopyable {
  virtual ~get_data()  {}
  virtual int operator()(int req, size_t col, variant& var) = 0;
}; // get_data

} } } } // brig::database::cubrid::detail

#endif // BRIG_DATABASE_CUBRID_DETAIL_GET_DATA_HPP
