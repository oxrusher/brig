// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQL_DROP_HPP
#define BRIG_DATABASE_DETAIL_SQL_DROP_HPP

#include <brig/database/detail/dialect.hpp>
#include <brig/global.hpp>
#include <brig/table_def.hpp>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

inline void sql_drop(dialect* dct, const table_def& tbl, std::vector<std::string>& sql)
{
  for (const auto& idx: tbl.indexes)
    if (index_type::Spatial == idx.type)
    {
      identifier lr(tbl.id);
      lr.qualifier = idx.columns.front();
      dct->sql_drop_spatial_index(lr, sql);
    }
  for (const auto& col: tbl.columns)
    if (column_type::Geometry == col.type)
      dct->sql_unregister_spatial_column(tbl, col.name, sql);
  sql.push_back("DROP TABLE " + dct->sql_identifier(tbl.id));
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQL_DROP_HPP
