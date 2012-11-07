// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_SRID_HPP
#define BRIG_DATABASE_DETAIL_GET_SRID_HPP

#include <brig/database/command.hpp>
#include <brig/database/detail/dialect.hpp>
#include <brig/database/numeric_cast.hpp>
#include <stdexcept>
#include <string>

namespace brig { namespace database { namespace detail {

inline int get_srid(dialect* dct, command* cmd, int epsg)
{
  using namespace std;

  const string sql(dct->sql_srid(epsg));
  if (sql.empty()) return epsg;
  cmd->exec(sql);
  vector<variant> row;
  int srid(-1);
  if (!cmd->fetch(row) || !numeric_cast(row[0], srid)) throw runtime_error("SRID error");
  return srid;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_SRID_HPP
