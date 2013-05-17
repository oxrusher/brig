// Andrew Naplavkov

#ifndef BRIG_GDAL_MULTITHREAD_INIT_HPP
#define BRIG_GDAL_MULTITHREAD_INIT_HPP

#include <brig/gdal/detail/lib.hpp>

namespace brig { namespace gdal {

// todo: MSVC November 2012 CTP - C++11 singleton problem
// C++ 11 §6.7 [stmt.dcl] p4:
// If control enters the declaration concurrently while the variable is being initialized, the concurrent execution shall wait for completion of the initialization.

inline void multithread_init()
{
  detail::lib::singleton();
}

} } // brig::gdal

#endif // BRIG_GDAL_MULTITHREAD_INIT_HPP
