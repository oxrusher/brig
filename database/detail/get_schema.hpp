// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_SCHEMA_HPP
#define BRIG_DATABASE_DETAIL_GET_SCHEMA_HPP

#include <brig/database/command.hpp>
#include <brig/database/detail/dialect.hpp>
#include <brig/string_cast.hpp>
#include <stdexcept>
#include <string>

namespace brig { namespace database { namespace detail {

inline std::string get_schema(dialect* dct, command* cmd)
{
  using namespace std;

  const string sql(dct->sql_schema());
  if (sql.empty()) return "";
  cmd->exec(sql);
  vector<variant> row;
  if (!cmd->fetch(row)) throw runtime_error("schema error");
  const string schema(string_cast<char>(row[0]));
  if (schema.empty()) throw runtime_error("schema error");
  return schema;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_SCHEMA_HPP
