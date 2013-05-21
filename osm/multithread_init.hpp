// Andrew Naplavkov

#ifndef BRIG_OSM_MULTITHREAD_INIT_HPP
#define BRIG_OSM_MULTITHREAD_INIT_HPP

#include <brig/osm/detail/lib.hpp>

namespace brig { namespace osm {

/*!
MSVC November 2012 CTP - C++11 singleton workaround.\n
C++11 6.7 [stmt.dcl] p4:\n
"If control enters the declaration concurrently while the variable is being initialized, the concurrent execution shall wait for completion of the initialization."\n
*/
inline void multithread_init()
{
  detail::lib::singleton();
}

} } // brig::osm

#endif // BRIG_OSM_MULTITHREAD_INIT_HPP
