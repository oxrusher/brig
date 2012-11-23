// Andrew Naplavkov

#ifndef BRIG_PROJ_SHARED_PJ_HPP
#define BRIG_PROJ_SHARED_PJ_HPP

#include <brig/proj/detail/lib.hpp>
#include <brig/string_cast.hpp>
#include <memory>
#include <stdexcept>
#include <string>

namespace brig { namespace proj {

class shared_pj
{
  struct resource {
    std::string def;
    projPJ pj;
    resource(const std::string& def_, projPJ pj_) : def(def_), pj(pj_)  {}
    ~resource()  { detail::lib::singleton().p_pj_free(pj); }
  }; // resource

  std::shared_ptr<resource> m_res;

  static std::string definition(bool cur_dir, int epsg);

public:
  shared_pj()  {}
  explicit shared_pj(int epsg);
  explicit shared_pj(const std::string& def);
  std::string definition() const  { return m_res? m_res->def: std::string(); }
  bool is_latlong() const  { return m_res? (detail::lib::singleton().p_pj_is_latlong(m_res->pj) != 0): false; }
  bool operator==(const shared_pj& r) const;
  operator projPJ() const  { return m_res? m_res->pj: 0; }
}; // shared_pj

inline std::string shared_pj::definition(bool cur_dir, int epsg)
{
  std::string str("+init=");
  if (cur_dir) str += "./";
  str += "epsg:" + string_cast<char>(epsg);
  return str;
}

inline shared_pj::shared_pj(int epsg)
{
  if (detail::lib::singleton().empty()) throw std::runtime_error("projection error");

  std::string def(definition(true, epsg));
  projPJ pj(detail::lib::singleton().p_pj_init_plus( def.c_str() ));
  if (!pj)
  {
    def = definition(false, epsg);
    pj = detail::lib::singleton().p_pj_init_plus( def.c_str() );
  }
  if (!pj)
    throw std::runtime_error("projection error");
  m_res = std::make_shared<resource>(def, pj);
}

inline shared_pj::shared_pj(const std::string& def)
{
  if (detail::lib::singleton().empty()) throw std::runtime_error("projection error");
  projPJ pj(detail::lib::singleton().p_pj_init_plus( def.c_str() ));
  if (!pj) throw std::runtime_error("projection error");
  m_res = std::make_shared<resource>(def, pj);
}

inline bool shared_pj::operator==(const shared_pj& r) const
{
  return m_res && r.m_res && !m_res->def.empty() && (m_res == r.m_res || m_res->def.compare(r.m_res->def) == 0);
} // shared_pj::

} } // brig::proj

#endif // BRIG_PROJ_SHARED_PJ_HPP
