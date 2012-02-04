// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_THREADED_LINK_HPP
#define BRIG_DATABASE_DETAIL_THREADED_LINK_HPP

#include <boost/thread/thread.hpp>
#include <brig/database/detail/link.hpp>
#include <brig/database/detail/linker.hpp>
#include <brig/database/detail/page.hpp>
#include <brig/database/detail/rowset.hpp>
#include <brig/detail/mediator.hpp>
#include <exception>
#include <functional>
#include <memory>
#include <sstream>

namespace brig { namespace database { namespace detail {

class threaded_link : public link
{
  struct mediator_impl : brig::detail::mediator<link>
  {
    page front, back;
    bool sync, done;

    mediator_impl() : sync(false), done(false)  {}
    void reset()  { front.reset(); sync = false; }
    void daemon(rowset* rs)  { if (sync && !done) done = !back.fill(rs); }
    void fill(rowset* rs)  { if (!sync) { back.reset(); sync = true; done = !back.fill(rs); } page::swap(front, back); }
  }; // mediator_impl

  std::shared_ptr<mediator_impl> m_med;

  static void worker(std::shared_ptr<linker> lkr, std::shared_ptr<mediator_impl> med);

public:
  explicit threaded_link(std::shared_ptr<linker> lkr) : m_med(new mediator_impl())  { boost::thread(worker, lkr, m_med); }
  virtual ~threaded_link()  { m_med->stop(); }
  virtual DBMS system();
  virtual void sql_parameter(size_t order, const column_detail& param_col, std::ostringstream& stream);
  virtual void sql_column(const column_detail& col, std::ostringstream& stream);
  virtual void exec(const std::string& sql, const std::vector<variant>& params, const std::vector<column_detail>& param_cols);
  virtual int64_t affected();
  virtual void columns(std::vector<std::string>& cols);
  virtual bool fetch(std::vector<variant>& row);
  virtual void start();
  virtual void commit();
  virtual void rollback();
}; // threaded_link

inline void threaded_link::worker(std::shared_ptr<linker> lkr, std::shared_ptr<mediator_impl> med)
{
  std::unique_ptr<link> lnk;
  try  { lnk = std::unique_ptr<link>(lkr->create()); }
  catch (const std::exception&)  { med->stop(std::current_exception()); return; }
  med->start();
  while (med->handle(lnk.get())) med->daemon(lnk.get());
}

inline DBMS threaded_link::system()
{
  auto bnd(std::bind(&link::system, std::placeholders::_1));
  mediator_impl::functor_impl<decltype(bnd), DBMS> func(bnd);
  m_med->call(&func);
  return func.m_res;
}

inline void threaded_link::sql_parameter(size_t order, const column_detail& param_col, std::ostringstream& stream)
{
  auto bnd(std::bind(&link::sql_parameter, std::placeholders::_1, order, std::cref(param_col), std::ref(stream)));
  mediator_impl::functor_impl<decltype(bnd), void> func(bnd);
  m_med->call(&func);
}

inline void threaded_link::sql_column(const column_detail& col, std::ostringstream& stream)
{
  auto bnd(std::bind(&link::sql_column, std::placeholders::_1, std::cref(col), std::ref(stream)));
  mediator_impl::functor_impl<decltype(bnd), void> func(bnd);
  m_med->call(&func);
}

inline void threaded_link::exec(const std::string& sql, const std::vector<variant>& params, const std::vector<column_detail>& param_cols)
{
  m_med->reset();
  auto bnd(std::bind(&link::exec, std::placeholders::_1, std::cref(sql), std::cref(params), std::cref(param_cols)));
  mediator_impl::functor_impl<decltype(bnd), void> func(bnd);
  m_med->call(&func);
}

inline int64_t threaded_link::affected()
{
  auto bnd(std::bind(&link::affected, std::placeholders::_1));
  mediator_impl::functor_impl<decltype(bnd), int64_t> func(bnd);
  m_med->call(&func);
  return func.m_res;
}

inline void threaded_link::columns(std::vector<std::string>& cols)
{
  auto bnd(std::bind(&link::columns, std::placeholders::_1, std::ref(cols)));
  mediator_impl::functor_impl<decltype(bnd), void> func(bnd);
  m_med->call(&func);
}

inline bool threaded_link::fetch(std::vector<variant>& row)
{
  if (m_med->front.empty())
  {
    auto bnd(std::bind(&mediator_impl::fill, m_med.get(), std::placeholders::_1));
    mediator_impl::functor_impl<decltype(bnd), void> func(bnd);
    m_med->call(&func);
  }
  return m_med->front.fetch(row);
}

inline void threaded_link::start()
{
  auto bnd(std::bind(&link::start, std::placeholders::_1));
  mediator_impl::functor_impl<decltype(bnd), void> func(bnd);
  m_med->call(&func);
}

inline void threaded_link::commit()
{
  auto bnd(std::bind(&link::commit, std::placeholders::_1));
  mediator_impl::functor_impl<decltype(bnd), void> func(bnd);
  m_med->call(&func);
}

inline void threaded_link::rollback()
{
  auto bnd(std::bind(&link::rollback, std::placeholders::_1));
  mediator_impl::functor_impl<decltype(bnd), void> func(bnd);
  m_med->call(&func);
} // threaded_link::

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_THREADED_LINK_HPP
