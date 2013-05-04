// Andrew Naplavkov

#ifndef BRIG_DATABASE_MYSQL_MULTITHREAD_INIT_HPP
#define BRIG_DATABASE_MYSQL_MULTITHREAD_INIT_HPP

#include <brig/database/mysql/detail/lib.hpp>

namespace brig { namespace database { namespace mysql {

inline void multithread_init()
{
  detail::lib::singleton();
}

} } } // brig::database::mysql

#endif // BRIG_DATABASE_MYSQL_MULTITHREAD_INIT_HPP
