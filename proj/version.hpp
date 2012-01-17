// Andrew Naplavkov

#ifndef BRIG_PROJ_VERSION_HPP
#define BRIG_PROJ_VERSION_HPP

#include <brig/proj/detail/lib.hpp>
#include <string>

namespace brig { namespace proj {

inline std::string version()
{
  return detail::lib::singleton().empty()? "": detail::lib::singleton().p_pj_get_release();
}

} } // brig::proj

#endif // BRIG_PROJ_VERSION_HPP
