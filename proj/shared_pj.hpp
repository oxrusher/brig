// Andrew Naplavkov

#ifndef BRIG_PROJ_SHARED_PJ_HPP
#define BRIG_PROJ_SHARED_PJ_HPP

#include <brig/proj/detail/def_from_epsg.hpp>
#include <brig/proj/detail/lib.hpp>
#include <memory>
#include <stdexcept>
#include <string>

namespace brig { namespace proj {

class shared_pj {
  std::shared_ptr<void> m_ptr;
  void init(const char* def); // todo: MSVC November 2012 CTP - delegating constructors problem
public:
  shared_pj()  {}
  explicit shared_pj(const char* def)  { init(def); }
  explicit shared_pj(const std::string& def)  { init(def.c_str()); }
  explicit shared_pj(int epsg)  { init(detail::def_from_epsg(epsg).c_str()); }
  std::string get_def() const  { return m_ptr? detail::lib::singleton().p_pj_get_def(m_ptr.get(), 0): ""; }
  bool is_latlong() const  { return m_ptr && detail::lib::singleton().p_pj_is_latlong(m_ptr.get()); }
  operator projPJ() const  { return m_ptr.get(); }
  bool operator ==(const shared_pj& r) const  { return m_ptr && r.m_ptr && (m_ptr.get() == r.m_ptr.get() || get_def().compare(r.get_def()) == 0); }
  bool operator !=(const shared_pj& r) const  { return !operator ==(r); }
}; // shared_pj

inline void shared_pj::init(const char* def)
{
  using namespace std;
  using namespace detail;
  if (!def || lib::singleton().empty()) throw runtime_error("projection error");
  projPJ pj(lib::singleton().p_pj_init_plus( def ));
  if (!pj) throw runtime_error("projection error");
  m_ptr = shared_ptr<void>(pj, [](void* ptr){ lib::singleton().p_pj_free(ptr); });
} // shared_pj::

} } // brig::proj

#endif // BRIG_PROJ_SHARED_PJ_HPP
