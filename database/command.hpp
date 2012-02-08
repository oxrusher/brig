// Andrew Naplavkov

#ifndef BRIG_DATABASE_COMMAND_HPP
#define BRIG_DATABASE_COMMAND_HPP

#include <brig/database/column_detail.hpp>
#include <brig/database/rowset.hpp>
#include <brig/database/variant.hpp>
#include <string>
#include <vector>

namespace brig { namespace database {

struct command : public rowset {
  virtual void exec
    ( const std::string& sql
    , const std::vector<variant>& params = std::vector<variant>()
    , const std::vector<column_detail>& param_cols = std::vector<column_detail>()
    ) = 0;
  virtual size_t affected() = 0;
}; // command

} } // brig::database

#endif // BRIG_DATABASE_COMMAND_HPP
