// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_EXTENT_HPP
#define BRIG_DATABASE_DETAIL_GET_EXTENT_HPP

#include <brig/boost/geometry.hpp>
#include <brig/database/command.hpp>
#include <brig/database/detail/dialect.hpp>
#include <brig/global.hpp>
#include <brig/numeric_cast.hpp>
#include <brig/table_def.hpp>
#include <iterator>
#include <stdexcept>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

inline boost::box get_extent(dialect* dct, command* cmd, const table_def& tbl)
{
  using namespace std;
  using namespace brig::boost;

  vector<string> query_columns(tbl.query_columns);
  if (query_columns.empty())
    for (auto col(begin(tbl.columns)); col != end(tbl.columns); ++col)
      if (column_type::Geometry == col->type) query_columns.push_back(col->name);
  if ( query_columns.size() != 1
    || column_type::Geometry != tbl[ query_columns.front() ]->type
     ) throw runtime_error("extent error");

  const string sql(dct->sql_extent(tbl, query_columns.front()));
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

  throw runtime_error("extent error");
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_EXTENT_HPP
