// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_THREADED_LINK_HPP
#define BRIG_DATABASE_DETAIL_THREADED_LINK_HPP

#include <boost/thread/thread.hpp>
#include <brig/database/detail/double_page.hpp>
#include <brig/database/detail/link.hpp>
#include <brig/database/detail/linker.hpp>
#include <brig/detail/mediator.hpp>
#include <exception>
#include <functional>
#include <memory>
#include <sstream>

namespace brig { namespace database { namespace detail {

class threaded_link : public link
{
  struct mediator : brig::detail::mediator<link>  { double_page dpg; };
  std::shared_ptr<mediator> m_med;
  static void worker(std::shared_ptr<linker> lkr, std::shared_ptr<mediator> med);
public:
  explicit threaded_link(std::shared_ptr<linker> lkr) : m_med(new mediator())  { boost::thread(worker, lkr, m_med); }
  virtual ~threaded_link()  { m_med->stop(); }
  virtual DBMS system();
  virtual void sql_parameter(size_t order, const column_detail& param_col, std::ostringstream& stream);
  virtual void sql_column(const column_detail& col, std::ostringstream& stream);
  virtual void exec(const std::string& sql, const std::vector<variant>& params, const std::vector<column_detail>& param_cols);
  virtual size_t affected();
  virtual void columns(std::vector<std::string>& cols);
  virtual bool fetch(std::vector<variant>& row);
  virtual void start();
  virtual void commit();
  virtual void rollback();
}; // threaded_link

inline void threaded_link::worker(std::shared_ptr<linker> lkr, std::shared_ptr<mediator> med)
{
  std::unique_ptr<link> lnk;
  try  { lnk = std::unique_ptr<link>(lkr->create()); }
  catch (const std::exception&)  { med->stop(std::current_exception()); return; }
  med->start();
  while (med->handle(lnk.get())) med->dpg.prefill(lnk.get());
}

inline DBMS threaded_link::system()
{
  auto bnd(mediator::bind(std::bind(&link::system, std::placeholders::_1)));
  m_med->call(&bnd);
  return bnd.r;
}

inline void threaded_link::sql_parameter(size_t order, const column_detail& param_col, std::ostringstream& stream)
{
  auto bnd(mediator::bind(std::bind(&link::sql_parameter, std::placeholders::_1, order, std::cref(param_col), std::ref(stream))));
  m_med->call(&bnd);
}

inline void threaded_link::sql_column(const column_detail& col, std::ostringstream& stream)
{
  auto bnd(mediator::bind(std::bind(&link::sql_column, std::placeholders::_1, std::cref(col), std::ref(stream))));
  m_med->call(&bnd);
}

inline void threaded_link::exec(const std::string& sql, const std::vector<variant>& params, const std::vector<column_detail>& param_cols)
{
  m_med->dpg.reset();
  auto bnd(mediator::bind(std::bind(&link::exec, std::placeholders::_1, std::cref(sql), std::cref(params), std::cref(param_cols))));
  m_med->call(&bnd);
}

inline size_t threaded_link::affected()
{
  auto bnd(mediator::bind(std::bind(&link::affected, std::placeholders::_1)));
  m_med->call(&bnd);
  return bnd.r;
}

inline void threaded_link::columns(std::vector<std::string>& cols)
{
  auto bnd(mediator::bind(std::bind(&link::columns, std::placeholders::_1, std::ref(cols))));
  m_med->call(&bnd);
}

inline bool threaded_link::fetch(std::vector<variant>& row)
{
  if (m_med->dpg.empty())
  {
    auto bnd(mediator::bind(std::bind(&double_page::fill, &m_med->dpg, std::placeholders::_1)));
    m_med->call(&bnd);
  }
  return m_med->dpg.fetch(row);
}

inline void threaded_link::start()
{
  auto bnd(mediator::bind(std::bind(&link::start, std::placeholders::_1)));
  m_med->call(&bnd);
}

inline void threaded_link::commit()
{
  auto bnd(mediator::bind(std::bind(&link::commit, std::placeholders::_1)));
  m_med->call(&bnd);
}

inline void threaded_link::rollback()
{
  auto bnd(mediator::bind(std::bind(&link::rollback, std::placeholders::_1)));
  m_med->call(&bnd);
} // threaded_link::

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_THREADED_LINK_HPP
