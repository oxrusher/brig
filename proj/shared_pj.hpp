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
  std::shared_ptr<void> m_ctx, m_pj;
  void init(const char* def); // todo: MSVC November 2012 CTP - delegating constructors problem
public:
  shared_pj()  {}
  explicit shared_pj(const char* def)  { init(def); }
  explicit shared_pj(const std::string& def)  { init(def.c_str()); }
  explicit shared_pj(int epsg)  { init(detail::def_from_epsg(epsg).c_str()); }
  std::string get_def() const  { return m_pj? detail::lib::singleton().p_pj_get_def(m_pj.get(), 0): ""; }
  bool is_latlong() const  { return m_pj && detail::lib::singleton().p_pj_is_latlong(m_pj.get()); }
  operator projPJ() const  { return m_pj.get(); }
  bool operator ==(const shared_pj& r) const  { return m_pj && r.m_pj && (m_pj.get() == r.m_pj.get() || get_def().compare(r.get_def()) == 0); }
  bool operator !=(const shared_pj& r) const  { return !operator ==(r); }
  shared_pj copy() const  { return shared_pj(get_def()); }
}; // shared_pj

inline void shared_pj::init(const char* def)
{
  using namespace std;
  using namespace detail;

  if (!def || lib::singleton().empty()) throw runtime_error("proj error");

  projCtx ctx(lib::singleton().p_pj_ctx_alloc());
  if (!ctx) throw runtime_error("proj error");
  m_ctx = shared_ptr<void>(ctx, [](void* ptr){ lib::singleton().p_pj_ctx_free(ptr); });

  projPJ pj(lib::singleton().p_pj_init_plus_ctx(ctx, def));
  if (!pj) throw runtime_error("proj error");
  m_pj = shared_ptr<void>(pj, [](void* ptr){ lib::singleton().p_pj_free(ptr); });
} // shared_pj::

} } // brig::proj

#endif // BRIG_PROJ_SHARED_PJ_HPP
