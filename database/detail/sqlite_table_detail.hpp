// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_SQLITE_TABLE_DETAIL_HPP
#define BRIG_DATABASE_DETAIL_SQLITE_TABLE_DETAIL_HPP

#include <algorithm>
#include <brig/database/column_detail.hpp>
#include <brig/database/command.hpp>
#include <brig/database/detail/is_ogc_type.hpp>
#include <brig/database/detail/sql_object.hpp>
#include <brig/database/global.hpp>
#include <brig/database/index_detail.hpp>
#include <brig/database/numeric_cast.hpp>
#include <brig/database/object.hpp>
#include <brig/database/table_detail.hpp>
#include <brig/detail/string_cast.hpp>
#include <memory>
#include <string>
#include <utility>

namespace brig { namespace database { namespace detail {

inline table_detail<column_detail> sqlite_table_detail(std::shared_ptr<command> cmd, const object& tbl)
{
  using namespace brig::detail;

  // columns
  table_detail<column_detail> meta;
  meta.table = tbl;
  std::vector<std::string> keys;
  std::vector<variant> row;
  cmd->exec("PRAGMA TABLE_INFO(" + sql_object(SQLite, tbl) + ')');
  while (cmd->fetch(row))
  {
    column_detail col;
    col.name = string_cast<char>(row[1]);
    col.type.name = string_cast<char>(row[2]);
    meta.columns.push_back(col);

    int key(0);
    if (numeric_cast(row[5], key) && key) keys.push_back(col.name);
  }

  if (!keys.empty())
  {
    index_detail pri_idx;
    pri_idx.type = Primary;
    pri_idx.columns = keys;
    meta.indexes.push_back(pri_idx);
    sort(keys.begin(), keys.end());
  }

  // indexes
  cmd->exec("PRAGMA INDEX_LIST(" + sql_object(SQLite, tbl) + ')');
  while (cmd->fetch(row))
  {
    index_detail idx;
    idx.index.name = string_cast<char>(row[1]);
    int unique(0);
    idx.type = (numeric_cast(row[2], unique) && !unique)? Duplicate: Unique;
    meta.indexes.push_back(idx);
  }

  // indexed columns
  for (size_t i(0); i < meta.indexes.size(); ++i)
  {
    if (meta.indexes[i].index.name.empty()) continue;

    cmd->exec("PRAGMA INDEX_INFO(" + sql_object(SQLite, meta.indexes[i].index) + ')');
    std::vector<std::string> cols;
    std::vector<std::pair<int, std::string>> seq_cols;
    while (cmd->fetch(row))
    {
      const std::string col(string_cast<char>(row[2]));
      cols.push_back(col);

      std::pair<int, std::string> seq_col;
      numeric_cast(row[0], seq_col.first);
      seq_col.second = col;
      seq_cols.push_back(seq_col);
    }
    std::sort(cols.begin(), cols.end());
    std::sort(seq_cols.begin(), seq_cols.end());
    for (size_t j(0); j < seq_cols.size(); ++j)
      meta.indexes[i].columns.push_back(seq_cols[j].second);

    if (keys.size() == cols.size() && std::equal(keys.begin(), keys.end(), cols.begin()))
    {
      keys.clear();
      meta.indexes[i].type = VoidIndex;
      meta.indexes[0].columns = std::move(meta.indexes[i].columns);
    }
  }
  auto end = std::remove_if(meta.indexes.begin(), meta.indexes.end(), [](const index_detail& idx){ return VoidIndex == idx.type; });
  meta.indexes.resize(std::distance(meta.indexes.begin(), end));

  // srid, epsg, spatial index
  for (size_t i(0); i < meta.columns.size(); ++i)
  {
    if (is_ogc_type(meta.columns[i].type.name))
    {
      cmd->exec("SELECT c.SRID, (CASE s.AUTH_NAME WHEN 'epsg' THEN s.AUTH_SRID ELSE NULL END) epsg, c.SPATIAL_INDEX_ENABLED FROM (SELECT SRID, SPATIAL_INDEX_ENABLED FROM GEOMETRY_COLUMNS WHERE F_TABLE_NAME = '" + tbl.name + "' AND F_GEOMETRY_COLUMN = '" + meta.columns[i].name + "') c LEFT JOIN SPATIAL_REF_SYS s ON c.SRID = s.SRID");
      if (cmd->fetch(row))
      {
        numeric_cast(row[0], meta.columns[i].srid);
        numeric_cast(row[1], meta.columns[i].epsg);
        int indexed(0);
        if (numeric_cast(row[2], indexed) && indexed == 1)
        {
          index_detail idx;
          idx.type = Spatial;
          idx.columns.push_back(meta.columns[i].name);
          meta.indexes.push_back(idx);
        }
      }
    }
  }
  return std::move(meta);
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQLITE_TABLE_DETAIL_HPP
