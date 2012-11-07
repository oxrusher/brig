// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_MBR_HPP
#define BRIG_DATABASE_DETAIL_GET_MBR_HPP

#include <brig/boost/geometry.hpp>
#include <brig/database/command.hpp>
#include <brig/database/detail/dialect.hpp>
#include <brig/database/global.hpp>
#include <brig/database/numeric_cast.hpp>
#include <brig/database/table_definition.hpp>
#include <stdexcept>
#include <string>

namespace brig { namespace database { namespace detail {

inline brig::boost::box get_mbr(dialect* dct, command* cmd, const table_definition& tbl, const std::string& col)
{
  using namespace std;
  using namespace brig::boost;

  if (Geometry != tbl[col]->type) throw runtime_error("datatype error");
  const string sql(dct->sql_mbr(tbl, col));
  if (sql.empty()) return box(point(-180, -90), point(180, 90)); // geodetic

  cmd->exec(sql);
  vector<variant> row;
  double xmin(0), ymin(0), xmax(0), ymax(0);
  if ( cmd->fetch(row)
    && numeric_cast(row[0], xmin)
    && numeric_cast(row[1], ymin)
    && numeric_cast(row[2], xmax)
    && numeric_cast(row[3], ymax)
     )
    return box(point(xmin, ymin), point(xmax, ymax));

  throw runtime_error("MBR error");
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_MBR_HPP
