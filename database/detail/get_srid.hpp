// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_SRID_HPP
#define BRIG_DATABASE_DETAIL_GET_SRID_HPP

#include <brig/database/command.hpp>
#include <brig/database/detail/dialect.hpp>
#include <brig/numeric_cast.hpp>
#include <string>

namespace brig { namespace database { namespace detail {

inline int get_srid(dialect* dct, command* cmd, int epsg)
{
  using namespace std;

  const string sql(dct->sql_srid(epsg));
  if (sql.empty()) return epsg;
  cmd->exec(sql);
  int srid(-1);
  vector<variant> row;
  return (cmd->fetch(row) && numeric_cast(row[0], srid))? srid: -1;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_SRID_HPP
