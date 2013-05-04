// Andrew Naplavkov

#ifndef BRIG_DATABASE_SQLITE_MULTITHREAD_INIT_HPP
#define BRIG_DATABASE_SQLITE_MULTITHREAD_INIT_HPP

#include <brig/database/sqlite/detail/lib.hpp>

namespace brig { namespace database { namespace sqlite {

inline void multithread_init()
{
  detail::lib::singleton();
}

} } } // brig::database::sqlite

#endif // BRIG_DATABASE_SQLITE_MULTITHREAD_INIT_HPP
