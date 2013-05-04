// Andrew Naplavkov

#ifndef BRIG_DATABASE_ODBC_MULTITHREAD_INIT_HPP
#define BRIG_DATABASE_ODBC_MULTITHREAD_INIT_HPP

#include <brig/database/odbc/detail/lib.hpp>

namespace brig { namespace database { namespace odbc {

inline void multithread_init()
{
  detail::lib::singleton();
}

} } } // brig::database::odbc

#endif // BRIG_DATABASE_ODBC_MULTITHREAD_INIT_HPP
