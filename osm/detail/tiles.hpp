// Andrew Naplavkov

#ifndef BRIG_OSM_DETAIL_TILES_HPP
#define BRIG_OSM_DETAIL_TILES_HPP

#include <boost/geometry/geometry.hpp>
#include <brig/boost/geometry.hpp>
#include <brig/osm/detail/tile.hpp>

namespace brig { namespace osm { namespace detail {

class tiles {
  const int m_zoom;
  const brig::boost::box m_env;
  tile m_next;
  bool find_in(const tile& tl);
public:
  tiles(int zoom, const brig::boost::box& env);
  bool fetch(tile& tl);
}; // tiles

inline bool tiles::find_in(const tile& tl)
{
  if (!tl.is_valid() || tl.z > m_zoom || !::boost::geometry::intersects(m_env, tl.get_box()))
    return false;

  if (tl.z < m_zoom)
  {
    for (int i(0); i < 2; ++i)
      for (int j(0); j < 2; ++j)
      {
        tile child(2*tl.x + i, 2*tl.y + j, tl.z + 1);
        if (find_in(child))
          return true;
      }
    return false;
  }

  m_next = tl;
  return true;
}

inline tiles::tiles(int zoom, const brig::boost::box& env)
  : m_zoom(zoom), m_env(env), m_next(0, 0, -1)
{
  find_in(tile(0, 0, 0));
}

inline bool tiles::fetch(tile& tl)
{
  if (!m_next.is_valid())
    return false;
  tl = m_next;

  while (m_next.is_valid())
  {
    const tile parent(m_next.x / 2, m_next.y /2, m_next.z - 1);
    if (parent.is_valid())
      for (int i(0); i < 2; ++i)
        for (int j(0); j < 2; ++j)
        {
          tile child(2*parent.x + i, 2*parent.y + j, parent.z + 1);
          if (((m_next.x < child.x) || (m_next.x == child.x && m_next.y < child.y)) && find_in(child))
            return true;
        }
    m_next = parent;
  }
  return true;
} // tiles::

} } } // brig::osm::detail

#endif // BRIG_OSM_DETAIL_TILES_HPP
