// Andrew Naplavkov

#ifndef BRIG_DATABASE_COMMAND_HPP
#define BRIG_DATABASE_COMMAND_HPP

#include <brig/database/column_definition.hpp>
#include <brig/database/command_traits.hpp>
#include <brig/database/global.hpp>
#include <brig/database/rowset.hpp>
#include <string>
#include <vector>

namespace brig { namespace database {

struct command : public rowset
{
  virtual void exec(const std::string& sql, const std::vector<column_definition>& params = std::vector<column_definition>()) = 0;
  virtual size_t affected() = 0;

  virtual void set_autocommit(bool autocommit) = 0;
  virtual void commit() = 0;

  virtual DBMS system() = 0;
  virtual command_traits traits()  { return command_traits(); }
}; // command

} } // brig::database

#endif // BRIG_DATABASE_COMMAND_HPP
