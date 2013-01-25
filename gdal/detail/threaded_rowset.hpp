// Andrew Naplavkov

#ifndef BRIG_GDAL_DETAIL_THREADED_ROWSET_HPP
#define BRIG_GDAL_DETAIL_THREADED_ROWSET_HPP

#include <brig/detail/double_page.hpp>
#include <brig/detail/mediator.hpp>
#include <brig/gdal/detail/datasource_allocator.hpp>
#include <brig/rowset.hpp>
#include <memory>
#include <thread>

namespace brig { namespace gdal { namespace detail {

class threaded_rowset : public brig::rowset {
  struct mediator : brig::detail::mediator<brig::rowset>  { brig::detail::double_page dpg; };
  std::shared_ptr<mediator> m_med;
public:
  threaded_rowset(std::shared_ptr<datasource_allocator> allocator, table_def tbl);
  ~threaded_rowset() override  { m_med->stop(); }
  std::vector<std::string> columns() override;
  bool fetch(std::vector<variant>& row) override;
}; // threaded_rowset

inline threaded_rowset::threaded_rowset(std::shared_ptr<datasource_allocator> allocator, table_def tbl) : m_med(new mediator())
{
  using namespace std;
  auto worker = [](shared_ptr<datasource_allocator> allocator, table_def tbl, shared_ptr<mediator> med)
  {
    unique_ptr<datasource> ds;
    try
    {
      ds = unique_ptr<datasource>(allocator->allocate(false));
      ds->select(tbl);
    }
    catch (const exception&)
    {
      med->stop(current_exception());
      return;
    }
    med->start();
    while (med->handle(ds.get())) med->dpg.prefill(ds.get());
  };
  thread t(worker, allocator, tbl, m_med);
  t.detach();
}

inline std::vector<std::string> threaded_rowset::columns()
{
  return m_med->call<std::vector<std::string>>(&brig::rowset::columns, std::placeholders::_1);
}

inline bool threaded_rowset::fetch(std::vector<variant>& row)
{
  if (m_med->dpg.empty()) m_med->call<void>(&brig::detail::double_page::fill, &m_med->dpg, std::placeholders::_1);
  return m_med->dpg.fetch(row);
} // threaded_rowset::

} } } // brig::gdal::detail

#endif // BRIG_GDAL_DETAIL_THREADED_ROWSET_HPP
