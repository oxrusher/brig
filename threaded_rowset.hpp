// Andrew Naplavkov

#ifndef BRIG_THREADED_ROWSET_HPP
#define BRIG_THREADED_ROWSET_HPP

#include <brig/detail/double_page.hpp>
#include <brig/detail/mediator.hpp>
#include <brig/rowset.hpp>
#include <memory>
#include <thread>

namespace brig {

class threaded_rowset : public rowset {
  struct mediator : detail::mediator<rowset>  { detail::double_page dpg; };
  std::shared_ptr<mediator> m_med;
public:
  explicit threaded_rowset(std::shared_ptr<rowset> rs);
  ~threaded_rowset() override  { m_med->stop(); }
  std::vector<std::string> columns() override;
  bool fetch(std::vector<variant>& row) override;
}; // threaded_rowset

inline threaded_rowset::threaded_rowset(std::shared_ptr<rowset> rs) : m_med(new mediator())
{
  using namespace std;
  auto worker = [](shared_ptr<rowset> rs, shared_ptr<mediator> med)
  {
    med->start();
    while (med->handle(rs.get())) med->dpg.prefill(rs.get());
  };
  thread t(worker, rs, m_med);
  t.detach();
}

inline std::vector<std::string> threaded_rowset::columns()
{
  return m_med->call<std::vector<std::string>>(&rowset::columns, std::placeholders::_1);
}

inline bool threaded_rowset::fetch(std::vector<variant>& row)
{
  if (m_med->dpg.empty()) m_med->call<void>(&detail::double_page::fill, &m_med->dpg, std::placeholders::_1);
  return m_med->dpg.fetch(row);
} // threaded_rowset::

} // brig

#endif // BRIG_THREADED_ROWSET_HPP
