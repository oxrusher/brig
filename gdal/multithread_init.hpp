// Andrew Naplavkov

#ifndef BRIG_GDAL_MULTITHREAD_INIT_HPP
#define BRIG_GDAL_MULTITHREAD_INIT_HPP

#include <brig/gdal/detail/lib.hpp>

namespace brig { namespace gdal {

inline void multithread_init()
{
  detail::lib::singleton();
}

} } // brig::gdal

#endif // BRIG_GDAL_MULTITHREAD_INIT_HPP
