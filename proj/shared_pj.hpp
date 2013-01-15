// Andrew Naplavkov

#ifndef BRIG_PROJ_SHARED_PJ_HPP
#define BRIG_PROJ_SHARED_PJ_HPP

#include <brig/proj/detail/def_from_epsg.hpp>
#include <brig/proj/detail/lib.hpp>
#include <brig/string_cast.hpp>
#include <memory>
#include <stdexcept>
#include <string>

namespace brig { namespace proj {

class shared_pj
{
  struct resource {
    projPJ pj;
    const std::string def;
    resource(projPJ pj_, const std::string& def_) : pj(pj_), def(def_)  {}
    ~resource()  { detail::lib::singleton().p_pj_free(pj); }
  }; // resource

  std::shared_ptr<resource> m_res;

  void init(const std::string& def); // todo: MSVC November 2012 CTP - delegating constructors problem

public:
  shared_pj()  {}
  explicit shared_pj(const std::string& def)  { init(def); }
  explicit shared_pj(int epsg)  { init(detail::def_from_epsg(epsg)); }
  std::string get_def() const  { return m_res? m_res->def: std::string(); }
  bool is_latlong() const  { return m_res && (detail::lib::singleton().p_pj_is_latlong(m_res->pj) != 0); }
  operator projPJ() const  { return m_res? m_res->pj: 0; }
  bool operator==(const shared_pj& r) const;
}; // shared_pj

inline void shared_pj::init(const std::string& def)
{
  using namespace std;
  using namespace detail;

  if (lib::singleton().empty()) throw runtime_error("projection error");
  projPJ pj(lib::singleton().p_pj_init_plus( def.c_str() ));
  if (!pj) throw runtime_error("projection error");
  m_res = make_shared<resource>(pj, def);
}

inline bool shared_pj::operator==(const shared_pj& r) const
{
  const std::string def(get_def());
  return !def.empty() && def.compare(r.get_def()) == 0;
} // shared_pj::

} } // brig::proj

#endif // BRIG_PROJ_SHARED_PJ_HPP
