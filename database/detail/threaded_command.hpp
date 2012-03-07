// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_THREADED_COMMAND_HPP
#define BRIG_DATABASE_DETAIL_THREADED_COMMAND_HPP

#include <boost/thread/thread.hpp>
#include <brig/database/command.hpp>
#include <brig/database/command_allocator.hpp>
#include <brig/database/detail/double_page.hpp>
#include <brig/detail/mediator.hpp>
#include <exception>
#include <functional>
#include <memory>

namespace brig { namespace database { namespace detail {

class threaded_command : public command
{
  struct mediator : brig::detail::mediator<command>  { double_page dpg; };
  std::shared_ptr<mediator> m_med;
  static void worker(std::shared_ptr<command_allocator> allocator, std::shared_ptr<mediator> med);
public:
  explicit threaded_command(std::shared_ptr<command_allocator> allocator) : m_med(new mediator())  { ::boost::thread(worker, allocator, m_med); }
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

inline void threaded_command::worker(std::shared_ptr<command_allocator> allocator, std::shared_ptr<mediator> med)
{
  std::unique_ptr<command> cmd;
  try  { cmd = std::unique_ptr<command>(allocator->allocate()); }
  catch (const std::exception&)  { med->stop(std::current_exception()); return; }
  med->start();
  while (med->handle(cmd.get())) med->dpg.prefill(cmd.get());
}

inline DBMS threaded_command::system()
{
  auto bnd(mediator::bind(std::bind(&command::system, std::placeholders::_1)));
  m_med->call(&bnd);
  return bnd.r;
}

inline std::string threaded_command::sql_parameter(size_t order, const column_definition& param_col)
{
  auto bnd(mediator::bind(std::bind(&command::sql_parameter, std::placeholders::_1, order, std::cref(param_col))));
  m_med->call(&bnd);
  return std::move(bnd.r);
}

inline std::string threaded_command::sql_column(const column_definition& col)
{
  auto bnd(mediator::bind(std::bind(&command::sql_column, std::placeholders::_1, std::cref(col))));
  m_med->call(&bnd);
  return std::move(bnd.r);
}

inline void threaded_command::exec(const std::string& sql, const std::vector<variant>& params, const std::vector<column_definition>& param_cols)
{
  m_med->dpg.clear();
  auto bnd(mediator::bind(std::bind(&command::exec, std::placeholders::_1, std::cref(sql), std::cref(params), std::cref(param_cols))));
  m_med->call(&bnd);
}

inline size_t threaded_command::affected()
{
  auto bnd(mediator::bind(std::bind(&command::affected, std::placeholders::_1)));
  m_med->call(&bnd);
  return bnd.r;
}

inline std::vector<std::string> threaded_command::columns()
{
  auto bnd(mediator::bind(std::bind(&command::columns, std::placeholders::_1)));
  m_med->call(&bnd);
  return std::move(bnd.r);
}

inline bool threaded_command::fetch(std::vector<variant>& row)
{
  if (m_med->dpg.empty())
  {
    auto bnd(mediator::bind(std::bind(&double_page::fill, &m_med->dpg, std::placeholders::_1)));
    m_med->call(&bnd);
  }
  return m_med->dpg.fetch(row);
}

inline void threaded_command::set_autocommit(bool autocommit)
{
  auto bnd(mediator::bind(std::bind(&command::set_autocommit, std::placeholders::_1, autocommit)));
  m_med->call(&bnd);
}

inline void threaded_command::commit()
{
  auto bnd(mediator::bind(std::bind(&command::commit, std::placeholders::_1)));
  m_med->call(&bnd);
} // threaded_command::

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_THREADED_COMMAND_HPP
