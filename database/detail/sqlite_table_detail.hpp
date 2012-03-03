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
#include <brig/unicode/lower_case.hpp>
#include <brig/unicode/transform.hpp>
#include <memory>
#include <string>
#include <utility>

namespace brig { namespace database { namespace detail {

inline table_detail<column_detail> sqlite_table_detail(std::shared_ptr<command> cmd, const object& tbl)
{
  using namespace brig::detail;
  using namespace brig::unicode;

  // columns
  table_detail<column_detail> res;
  res.table = tbl;
  std::vector<std::string> keys;
  std::vector<variant> row;
  cmd->exec("PRAGMA TABLE_INFO(" + sql_object(SQLite, tbl) + ')');
  while (cmd->fetch(row))
  {
    column_detail col;
    col.name = string_cast<char>(row[1]);
    col.type.name = string_cast<char>(row[2]);
    col.lower_case_type.name = transform<std::string>(col.type.name, lower_case);
    res.columns.push_back(col);

    int key(0);
    if (numeric_cast(row[5], key) && key) keys.push_back(col.name);
  }
  if (res.columns.empty()) throw std::runtime_error("table error");

  if (!keys.empty())
  {
    index_detail pri_idx;
    pri_idx.type = Primary;
    pri_idx.columns = keys;
    res.indexes.push_back(pri_idx);
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
    res.indexes.push_back(idx);
  }

  // indexed columns
  for (size_t i(0); i < res.indexes.size(); ++i)
  {
    if (res.indexes[i].index.name.empty()) continue;

    cmd->exec("PRAGMA INDEX_INFO(" + sql_object(SQLite, res.indexes[i].index) + ')');
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
      res.indexes[i].columns.push_back(seq_cols[j].second);

    if (keys.size() == cols.size() && std::equal(keys.begin(), keys.end(), cols.begin()))
    {
      keys.clear();
      res.indexes[i].type = VoidIndex;
      res.indexes[0].columns = std::move(res.indexes[i].columns);
    }
  }
  auto end = std::remove_if(res.indexes.begin(), res.indexes.end(), [](const index_detail& idx){ return VoidIndex == idx.type; });
  res.indexes.resize(std::distance(res.indexes.begin(), end));

  // srid, epsg, spatial index
  for (size_t i(0); i < res.columns.size(); ++i)
  {
    if (is_ogc_type(res.columns[i].lower_case_type.name))
    {
      cmd->exec("SELECT c.SRID, (CASE s.AUTH_NAME WHEN 'epsg' THEN s.AUTH_SRID ELSE NULL END) epsg, c.SPATIAL_INDEX_ENABLED FROM (SELECT SRID, SPATIAL_INDEX_ENABLED FROM GEOMETRY_COLUMNS WHERE F_TABLE_NAME = '" + tbl.name + "' AND F_GEOMETRY_COLUMN = '" + res.columns[i].name + "') c LEFT JOIN SPATIAL_REF_SYS s ON c.SRID = s.SRID");
      if (cmd->fetch(row))
      {
        numeric_cast(row[0], res.columns[i].srid);
        numeric_cast(row[1], res.columns[i].epsg);
        int indexed(0);
        if (numeric_cast(row[2], indexed) && indexed == 1)
        {
          index_detail idx;
          idx.type = Spatial;
          idx.columns.push_back(res.columns[i].name);
          res.indexes.push_back(idx);
        }
      }
    }
  }
  return res;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_SQLITE_TABLE_DETAIL_HPP
