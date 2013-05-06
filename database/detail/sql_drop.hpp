// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_DROP_HPP
#define BRIG_DATABASE_DETAIL_SQL_DROP_HPP

#include <brig/database/detail/dialect.hpp>
#include <brig/global.hpp>
#include <brig/table_def.hpp>
#include <iterator>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

inline void sql_drop(dialect* dct, const table_def& tbl, std::vector<std::string>& sql)
{
  using namespace std;

  for (auto idx(begin(tbl.indexes)); idx != end(tbl.indexes); ++idx)
    if (index_type::Spatial == idx->type)
    {
      identifier lr(tbl.id);
      lr.qualifier = idx->columns.front();
      dct->sql_drop_spatial_index(lr, sql);
    }
  for (auto col(begin(tbl.columns)); col != end(tbl.columns); ++col)
    if (column_type::Geometry == col->type)
    {
      identifier lr(tbl.id);
      lr.qualifier = col->name;
      dct->sql_unregister_spatial_column(lr, sql);
    }
  sql.push_back("DROP TABLE " + dct->sql_identifier(tbl.id));
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_DROP_HPP
