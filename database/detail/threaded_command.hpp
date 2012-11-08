// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_THREADED_COMMAND_HPP
#define BRIG_DATABASE_DETAIL_THREADED_COMMAND_HPP

#include <brig/database/command.hpp>
#include <brig/database/command_allocator.hpp>
#include <brig/database/detail/double_page.hpp>
#include <brig/detail/mediator.hpp>
#include <exception>
#include <memory>
#include <thread>

namespace brig { namespace database { namespace detail {

class threaded_command : public command {
  struct mediator : brig::detail::mediator<command>  { double_page dpg; };
  std::shared_ptr<mediator> m_med;
public:
  explicit threaded_command(std::shared_ptr<command_allocator> allocator);
  ~threaded_command() override  { m_med->stop(); }
  void exec(const std::string& sql, const std::vector<column_definition>& params) override;
  size_t affected() override;
  std::vector<std::string> columns() override;
  bool fetch(std::vector<variant>& row) override;
  void set_autocommit(bool autocommit) override;
  void commit() override;
  DBMS system() override;
  command_traits traits() override;
}; // threaded_command

inline threaded_command::threaded_command(std::shared_ptr<command_allocator> allocator) : m_med(new mediator())
{
  using namespace std;
  auto worker = [](shared_ptr<command_allocator> allocator, shared_ptr<mediator> med)
  {
    unique_ptr<command> cmd;
    try  { cmd = unique_ptr<command>(allocator->allocate()); }
    catch (const exception&)  { med->stop(current_exception()); return; }
    med->start();
    while (med->handle(cmd.get())) med->dpg.prefill(cmd.get());
  };
  thread t(worker, allocator, m_med);
  t.detach();
}

inline void threaded_command::exec(const std::string& sql, const std::vector<column_definition>& params)
{
  using namespace std;
  m_med->dpg.clear();
  m_med->call<void>(&command::exec, placeholders::_1, cref(sql), cref(params));
}

inline size_t threaded_command::affected()
{
  return m_med->call<size_t>(&command::affected, std::placeholders::_1);
}

inline std::vector<std::string> threaded_command::columns()
{
  using namespace std;
  return m_med->call<vector<string>>(&command::columns, placeholders::_1);
}

inline bool threaded_command::fetch(std::vector<variant>& row)
{
  if (m_med->dpg.empty())
    m_med->call<void>(&double_page::fill, &m_med->dpg, std::placeholders::_1);
  return m_med->dpg.fetch(row);
}

inline void threaded_command::set_autocommit(bool autocommit)
{
  m_med->call<void>(&command::set_autocommit, std::placeholders::_1, autocommit);
}

inline void threaded_command::commit()
{
  m_med->call<void>(&command::commit, std::placeholders::_1);
}

inline DBMS threaded_command::system()
{
  return m_med->call<DBMS>(&command::system, std::placeholders::_1);
}

inline command_traits threaded_command::traits()
{
  return m_med->call<command_traits>(&command::traits, std::placeholders::_1);
} // threaded_command::

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_THREADED_COMMAND_HPP
