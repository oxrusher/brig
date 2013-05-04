// Andrew Naplavkov

#ifndef BRIG_OSM_MULTITHREAD_INIT_HPP
#define BRIG_OSM_MULTITHREAD_INIT_HPP

#include <brig/osm/detail/lib.hpp>

namespace brig { namespace osm {

inline void multithread_init()
{
  detail::lib::singleton();
}

} } // brig::osm

#endif // BRIG_OSM_MULTITHREAD_INIT_HPP
