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

inline void sqlite_table_detail(std::shared_ptr<command> cmd, const object& tbl, table_detail<column_detail>& meta)
{
  using namespace brig::detail;

  // columns
  std::vector<std::string> keys;
  std::vector<variant> row;
  cmd->exec("PRAGMA TABLE_INFO(" + sql_object(SQLite, tbl) + ')');
  while (cmd->fetch(row))
  {
    column_detail col;
    col.name = string_cast<char>(row[1]);
    col.type.name = string_cast<char>(row[2]);
    meta.cols.push_back(col);

    int key(0);
    if (numeric_cast(row[5], key) && key) keys.push_back(col.name);
  }

  if (!keys.empty())
  {
    index_detail pri_idx;
    pri_idx.type = Primary;
    pri_idx.cols = keys;
    meta.idxs.push_back(pri_idx);
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
    meta.idxs.push_back(idx);
  }

  // indexed columns
  for (size_t i(0); i < meta.idxs.size(); ++i)
  {
    if (meta.idxs[i].index.name.empty()) continue;
    cmd->exec("PRAGMA INDEX_INFO(" + sql_object(SQLite, meta.idxs[i].index) + ')');
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

    for (size_t j(0); j <seq_cols.size(); ++j)
      meta.idxs[i].cols.push_back(seq_cols[j].second);

    if (keys.size() == cols.size() && equal(keys.begin(), keys.end(), cols.begin()))
    {
      keys.clear();
      meta.idxs[i].type = VoidIndex;
      meta.idxs[0].cols = move(meta.idxs[i].cols);
    }
  }

  std::remove_if(meta.idxs.begin(), meta.idxs.end(), [](const index_detail& idx){ return VoidIndex == idx.type; });

  // srid, epsg, spatial index
  for (size_t i(0); i < meta.cols.size(); ++i)
  {
    if (is_ogc_type(meta.cols[i].type.name))
    {
      cmd->exec("SELECT c.SRID, (CASE s.AUTH_NAME WHEN 'epsg' THEN s.AUTH_SRID ELSE NULL END) epsg, c.SPATIAL_INDEX_ENABLED FROM (SELECT SRID, SPATIAL_INDEX_ENABLED FROM GEOMETRY_COLUMNS WHERE F_TABLE_NAME = '" + tbl.name + "' AND F_GEOMETRY_COLUMN = '" + meta.cols[i].name + "') c LEFT JOIN SPATIAL_REF_SYS s ON c.SRID = s.SRID");
      if (cmd->fetch(row))
      {
        numeric_cast(row[0], meta.cols[i].srid);
        numeric_cast(row[1], meta.cols[i].epsg);
        int indexed(0);
        if (numeric_cast(row[2], indexed) && indexed == 1)
        {
          index_detail idx;
          idx.type = Spatial;
          idx.cols.push_back(meta.cols[i].name);
          meta.idxs.push_back(idx);
        }
      }
    }
  }
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQLITE_TABLE_DETAIL_HPP
