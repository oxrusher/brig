// Andrew Naplavkov

#ifndef BRIG_PROJ_MULTITHREAD_INIT_HPP
#define BRIG_PROJ_MULTITHREAD_INIT_HPP

#include <brig/proj/detail/lib.hpp>

namespace brig { namespace proj {

// todo: MSVC November 2012 CTP - C++11 singleton problem
// C++ 11 §6.7 [stmt.dcl] p4:
// If control enters the declaration concurrently while the variable is being initialized, the concurrent execution shall wait for completion of the initialization.

inline void multithread_init()
{
  detail::lib::singleton();
}

} } // brig::proj

#endif // BRIG_PROJ_MULTITHREAD_INIT_HPP
