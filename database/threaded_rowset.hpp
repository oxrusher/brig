// Andrew Naplavkov

#ifndef BRIG_DATABASE_THREADED_ROWSET_HPP
#define BRIG_DATABASE_THREADED_ROWSET_HPP

#include <boost/thread/thread.hpp>
#include <brig/database/detail/double_page.hpp>
#include <brig/database/rowset.hpp>
#include <brig/detail/mediator.hpp>
#include <functional>
#include <memory>

namespace brig { namespace database {

class threaded_rowset : public rowset {
  struct mediator : brig::detail::mediator<rowset>  { detail::double_page dpg; };
  std::shared_ptr<mediator> m_med;
  static void worker(std::shared_ptr<rowset> rs, std::shared_ptr<mediator> med);
public:
  explicit threaded_rowset(std::shared_ptr<rowset> rs) : m_med(new mediator())  { ::boost::thread(worker, rs, m_med); }
  virtual ~threaded_rowset()  { m_med->stop(); }
  virtual std::vector<std::string> columns();
  virtual bool fetch(std::vector<variant>& row);
}; // threaded_rowset

inline void threaded_rowset::worker(std::shared_ptr<rowset> rs, std::shared_ptr<mediator> med)
{
  med->start();
  while (med->handle(rs.get())) med->dpg.prefill(rs.get());
}

inline std::vector<std::string> threaded_rowset::columns()
{
  auto bnd(mediator::bind(std::bind(&rowset::columns, std::placeholders::_1)));
  m_med->call(&bnd);
  return std::move(bnd.r);
}

inline bool threaded_rowset::fetch(std::vector<variant>& row)
{
  if (m_med->dpg.empty())
  {
    auto bnd(mediator::bind(std::bind(&detail::double_page::fill, &m_med->dpg, std::placeholders::_1)));
    m_med->call(&bnd);
  }
  return m_med->dpg.fetch(row);
} // threaded_rowset::

} } // brig::database

#endif // BRIG_DATABASE_THREADED_ROWSET_HPP
