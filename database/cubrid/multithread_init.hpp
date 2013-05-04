// Andrew Naplavkov

#ifndef BRIG_DATABASE_CUBRID_MULTITHREAD_INIT_HPP
#define BRIG_DATABASE_CUBRID_MULTITHREAD_INIT_HPP

#include <brig/database/cubrid/detail/lib.hpp>

namespace brig { namespace database { namespace cubrid {

inline void multithread_init()
{
  detail::lib::singleton();
}

} } } // brig::database::cubrid

#endif // BRIG_DATABASE_CUBRID_MULTITHREAD_INIT_HPP
