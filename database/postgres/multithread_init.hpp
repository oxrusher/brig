// Andrew Naplavkov

#ifndef BRIG_DATABASE_POSTGRES_MULTITHREAD_INIT_HPP
#define BRIG_DATABASE_POSTGRES_MULTITHREAD_INIT_HPP

#include <brig/database/postgres/detail/lib.hpp>

namespace brig { namespace database { namespace postgres {

inline void multithread_init()
{
  detail::lib::singleton();
}

} } } // brig::database::postgres

#endif // BRIG_DATABASE_POSTGRES_MULTITHREAD_INIT_HPP
