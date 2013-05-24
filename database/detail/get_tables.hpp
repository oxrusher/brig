// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_TABLES_HPP
#define BRIG_DATABASE_DETAIL_GET_TABLES_HPP

#include <brig/identifier.hpp>
#include <brig/database/command.hpp>
#include <brig/database/detail/dialect.hpp>
#include <brig/string_cast.hpp>
#include <vector>

namespace brig { namespace database { namespace detail {

inline std::vector<identifier> get_tables(dialect* dct, command* cmd)
{
  using namespace std;

  vector<identifier> res;
  cmd->exec(dct->sql_tables() + " ORDER BY 1, 2");
  vector<variant> row;
  while (cmd->fetch(row))
  {
    identifier tbl = { string_cast<char>(row[0]), string_cast<char>(row[1]) };
    res.push_back(tbl);
  }
  return res;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_TABLES_HPP
