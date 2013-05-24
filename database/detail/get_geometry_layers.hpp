// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_GEOMETRY_LAYERS_HPP
#define BRIG_DATABASE_DETAIL_GET_GEOMETRY_LAYERS_HPP

#include <brig/identifier.hpp>
#include <brig/database/command.hpp>
#include <brig/database/detail/dialect.hpp>
#include <brig/string_cast.hpp>
#include <vector>

namespace brig { namespace database { namespace detail {

inline std::vector<identifier> get_geometry_layers(dialect* dct, command* cmd)
{
  using namespace std;
  const string sql(dct->sql_geometries());
  if (sql.empty()) return vector<identifier>();
  cmd->exec(sql);
  vector<identifier> res;
  vector<variant> row;
  while (cmd->fetch(row))
  {
    identifier lr = { string_cast<char>(row[0]), string_cast<char>(row[1]), string_cast<char>(row[2]) };
    res.push_back(lr);
  }
  return res;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_GEOMETRY_LAYERS_HPP
