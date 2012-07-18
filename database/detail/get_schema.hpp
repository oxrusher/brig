// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_GET_SCHEMA_HPP
#define BRIG_DATABASE_DETAIL_GET_SCHEMA_HPP

#include <brig/database/command.hpp>
#include <brig/database/global.hpp>
#include <brig/database/variant.hpp>
#include <brig/string_cast.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

inline std::string get_schema(std::shared_ptr<command> cmd)
{
  std::string sql;
  switch (cmd->system())
  {
  default: throw std::runtime_error("SQL error");
  case CUBRID: sql = "SELECT CURRENT_USER"; break;
  case DB2: sql = "VALUES RTRIM(CURRENT_SCHEMA)"; break;
  case Informix: sql = "SELECT RTRIM(USER) FROM sysmaster:systables WHERE tabid = 1"; break;
  case MS_SQL: sql = "SELECT SCHEMA_NAME()"; break;
  case MySQL: sql = "select schema()"; break;
  case Oracle: sql = "SELECT SYS_CONTEXT('USERENV','SESSION_SCHEMA') FROM DUAL"; break;
  case Postgres: sql = "select current_schema()"; break;
  case SQLite: return "";
  }

  cmd->exec(sql);
  std::vector<variant> row;
  if (!cmd->fetch(row)) throw std::runtime_error("SQL error");
  return string_cast<char>(row[0]);
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_GET_SCHEMA_HPP
