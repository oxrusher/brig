// Andrew Naplavkov

#ifndef BRIG_DATABASE_COMMAND_TRAITS_HPP
#define BRIG_DATABASE_COMMAND_TRAITS_HPP

#include <brig/string_cast.hpp>
#include <string>

namespace brig { namespace database {

struct command_traits {
  char parameter_prefix;
  bool parameter_suffix;
  bool readable_geometry;
  bool writable_geometry;

  command_traits() : parameter_prefix('?'), parameter_suffix(false), readable_geometry(false), writable_geometry(false)  {}
  std::string sql_parameter_marker(size_t order) const;
}; // command_traits

inline std::string command_traits::sql_parameter_marker(size_t order) const
{
  if (parameter_suffix) return parameter_prefix + string_cast<char>(order + 1);
  else return std::string(1, parameter_prefix);
} // command_traits::

} } // brig::database

#endif // BRIG_DATABASE_COMMAND_TRAITS_HPP
