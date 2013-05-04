// Andrew Naplavkov

#ifndef BRIG_PROJ_MULTITHREAD_INIT_HPP
#define BRIG_PROJ_MULTITHREAD_INIT_HPP

#include <brig/proj/detail/lib.hpp>

namespace brig { namespace proj {

inline void multithread_init()
{
  detail::lib::singleton();
}

} } // brig::proj

#endif // BRIG_PROJ_MULTITHREAD_INIT_HPP
