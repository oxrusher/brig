// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_INSERTER_HPP
#define BRIG_DATABASE_DETAIL_INSERTER_HPP

#include <brig/database/command.hpp>
#include <brig/database/detail/dialect_factory.hpp>
#include <brig/detail/get_columns.hpp>
#include <brig/inserter.hpp>
#include <brig/table_def.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

namespace brig { namespace database { namespace detail {

template <typename Deleter>
class inserter : public brig::inserter {
  std::unique_ptr<command, Deleter> m_cmd;
  std::string m_sql;
  std::vector<column_def> m_params;
public:
  inserter(command* cmd, Deleter&& deleter, const table_def& tbl);
  void insert(std::vector<variant>& row) override;
  void flush() override  { m_cmd->commit(); }
}; // inserter

template <typename Deleter>
inserter<Deleter>::inserter(command* cmd, Deleter&& deleter, const table_def& tbl) : m_cmd(cmd, std::move(deleter))
{
  using namespace std;
  unique_ptr<dialect> dct(dialect_factory(m_cmd->system()));
  vector<column_def> cols = tbl.query_columns.empty()? tbl.columns: brig::detail::get_columns(tbl.columns, tbl.query_columns);
  string prefix, suffix;
  for (auto col(begin(cols)); col != end(cols); ++col)
  {
    if (col != begin(cols))
    {
      prefix += ", ";
      suffix += ", ";
    }
    prefix += dct->sql_identifier(col->name);
    suffix += dct->sql_parameter(cmd, *col, m_params.size());
    m_params.push_back(*col);
  }
  m_sql = "INSERT INTO " + dct->sql_identifier(tbl.id) + "(" + prefix + ") VALUES(" + suffix + ")";
  m_cmd->set_autocommit(false);
}

template <typename Deleter>
void inserter<Deleter>::insert(std::vector<variant>& row)
{
  if (row.size() != m_params.size())
    throw std::runtime_error("insert error");
  for (size_t i(0); i < m_params.size(); ++i)
    ::boost::swap(row[i], m_params[i].query_value);
  m_cmd->exec(m_sql, m_params);
} // inserter::

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_INSERTER_HPP
