// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_THREADED_COMMAND_HPP
#define BRIG_DATABASE_DETAIL_THREADED_COMMAND_HPP

#include <brig/database/command.hpp>
#include <brig/database/command_allocator.hpp>
#include <brig/database/detail/double_page.hpp>
#include <brig/detail/deleter.hpp>
#include <brig/detail/mediator.hpp>
#include <memory>
#include <thread>

namespace brig { namespace database { namespace detail {

class threaded_command : public command {
  struct mediator : brig::detail::mediator<command>  { double_page dpg; };
  std::shared_ptr<mediator> m_med;
public:
  explicit threaded_command(std::shared_ptr<command_allocator> allocator);
  virtual ~threaded_command()  { m_med->stop(); }
  virtual DBMS system();
  virtual std::string sql_parameter(size_t order, const column_definition& param_col);
  virtual std::string sql_column(const column_definition& col);
  virtual void exec(const std::string& sql, const std::vector<variant>& params, const std::vector<column_definition>& param_cols);
  virtual size_t affected();
  virtual std::vector<std::string> columns();
  virtual bool fetch(std::vector<variant>& row);
  virtual void set_autocommit(bool autocommit);
  virtual void commit();
}; // threaded_command

inline threaded_command::threaded_command(std::shared_ptr<command_allocator> allocator) : m_med(new mediator())
{
  auto worker = [](std::shared_ptr<command_allocator> allocator, std::shared_ptr<mediator> med)
  {
    std::unique_ptr<command, brig::detail::deleter<command, command_allocator>> cmd(allocator->allocate(), brig::detail::deleter<command, command_allocator>(allocator));
    med->start();
    while (med->handle(cmd.get())) med->dpg.prefill(cmd.get());
  };
  std::thread t(worker, allocator, m_med);
  t.detach();
}

inline DBMS threaded_command::system()
{
  return m_med->call(&command::system, std::placeholders::_1);
}

inline std::string threaded_command::sql_parameter(size_t order, const column_definition& param_col)
{
  return m_med->call(&command::sql_parameter, std::placeholders::_1, order, std::cref(param_col));
}

inline std::string threaded_command::sql_column(const column_definition& col)
{
  return m_med->call(&command::sql_column, std::placeholders::_1, std::cref(col));
}

inline void threaded_command::exec(const std::string& sql, const std::vector<variant>& params, const std::vector<column_definition>& param_cols)
{
  m_med->dpg.clear();
  m_med->call(&command::exec, std::placeholders::_1, std::cref(sql), std::cref(params), std::cref(param_cols));
}

inline size_t threaded_command::affected()
{
  return m_med->call(&command::affected, std::placeholders::_1);
}

inline std::vector<std::string> threaded_command::columns()
{
  return m_med->call(&command::columns, std::placeholders::_1);
}

inline bool threaded_command::fetch(std::vector<variant>& row)
{
  if (m_med->dpg.empty())
    m_med->call(&double_page::fill, &m_med->dpg, std::placeholders::_1);
  return m_med->dpg.fetch(row);
}

inline void threaded_command::set_autocommit(bool autocommit)
{
  m_med->call(&command::set_autocommit, std::placeholders::_1, autocommit);
}

inline void threaded_command::commit()
{
  m_med->call(&command::commit, std::placeholders::_1);
} // threaded_command::

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_THREADED_COMMAND_HPP
