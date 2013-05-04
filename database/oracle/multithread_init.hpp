// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_MULTITHREAD_INIT_HPP
#define BRIG_DATABASE_ORACLE_MULTITHREAD_INIT_HPP

#include <brig/database/oracle/detail/lib.hpp>

namespace brig { namespace database { namespace oracle {

inline void multithread_init()
{
  detail::lib::singleton();
}

} } } // brig::database::oracle

#endif // BRIG_DATABASE_ORACLE_MULTITHREAD_INIT_HPP
