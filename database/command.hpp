// Andrew Naplavkov

#ifndef BRIG_DATABASE_COMMAND_HPP
#define BRIG_DATABASE_COMMAND_HPP

#include <brig/column_def.hpp>
#include <brig/database/dbms.hpp>
#include <brig/rowset.hpp>
#include <string>
#include <vector>

namespace brig { namespace database {

struct command : rowset {
  virtual void exec(const std::string& sql, const std::vector<column_def>& params = std::vector<column_def>()) = 0;
  virtual void exec_batch(const std::string& sql) = 0;

  virtual void set_autocommit(bool autocommit) = 0;
  virtual void commit() = 0;

  virtual DBMS system() = 0;
  virtual std::string sql_param(size_t /*order*/)  { return "?"; }
  virtual bool readable_geom()  { return false; }
  virtual bool writable_geom()  { return false; }
}; // command

} } // brig::database

#endif // BRIG_DATABASE_COMMAND_HPP
