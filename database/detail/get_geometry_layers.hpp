// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_GEOMETRY_LAYERS_HPP
#define BRIG_DATABASE_DETAIL_GET_GEOMETRY_LAYERS_HPP

#include <brig/database/command.hpp>
#include <brig/database/detail/dialect.hpp>
#include <brig/database/identifier.hpp>
#include <brig/string_cast.hpp>
#include <vector>

namespace brig { namespace database { namespace detail {

inline std::vector<identifier> get_geometry_layers(dialect* dct, command* cmd)
{
  using namespace std;

  cmd->exec(dct->sql_geometries());
  vector<identifier> res;
  vector<variant> row;
  while (cmd->fetch(row))
  {
    identifier lr;
    lr.schema = string_cast<char>(row[0]);
    lr.name = string_cast<char>(row[1]);
    lr.qualifier = string_cast<char>(row[2]);
    res.push_back(lr);
  }
  return res;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_GEOMETRY_LAYERS_HPP
