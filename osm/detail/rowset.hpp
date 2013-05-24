// Andrew Naplavkov

#ifndef BRIG_OSM_DETAIL_ROWSET_HPP
#define BRIG_OSM_DETAIL_ROWSET_HPP

#include <brig/boost/geometry.hpp>
#include <brig/boost/as_binary.hpp>
#include <brig/global.hpp>
#include <brig/osm/detail/lib.hpp>
#include <brig/osm/detail/tile.hpp>
#include <brig/osm/detail/tiles.hpp>
#include <brig/osm/layer.hpp>
#include <brig/rowset.hpp>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace brig { namespace osm { namespace detail {

class rowset : public brig::rowset
{
  struct data_t {
    tile tl;
    blob_t* rast;
    data_t() : tl(0, 0, 0), rast(0)  {}
  }; // data_t

  std::shared_ptr<layer> m_lr;
  const std::vector<bool> m_cols;
  int m_rows;
  tiles m_tls;
  CURLM* m_hnd;
  std::unordered_map<CURL*, data_t> m_pg;

  static size_t write(void* ptr, size_t size, size_t nmemb, blob_t* blob);
  static void check(CURLcode r);
  static void check(CURLMcode r);

  void add_files();

public:
  rowset(std::shared_ptr<layer> lr, const std::vector<bool>& cols, int zoom, const boost::box& env, int rows);
  ~rowset() override;
  std::vector<std::string> columns() override;
  bool fetch(std::vector<variant>& row) override;
}; // rowset

inline size_t rowset::write(void* ptr, size_t size, size_t nmemb, blob_t* blob)
{
  const uint8_t* bytes(static_cast<const uint8_t*>(ptr));
  for (size_t i(0), count(size * nmemb); i < count; ++i)
    blob->push_back(bytes[i]);
  return nmemb;
}

inline void rowset::check(CURLcode r)
{
  if (CURLE_OK == r) return;
  std::string msg(lib::singleton().p_curl_easy_strerror(r));
  if (msg.empty()) msg = "cURL error";
  throw std::runtime_error(msg);
}

inline void rowset::check(CURLMcode r)
{
  if (CURLM_OK == r) return;
  std::string msg(lib::singleton().p_curl_multi_strerror(r));
  if (msg.empty()) msg = "cURL error";
  throw std::runtime_error(msg);
}

inline rowset::rowset(std::shared_ptr<layer> lr, const std::vector<bool>& cols, int zoom, const boost::box& env, int rows)
  : m_lr(lr), m_cols(cols), m_rows(rows), m_tls(zoom, env), m_hnd(0)
{
  if (lib::singleton().empty()) throw std::runtime_error("cURL error");
  m_hnd = lib::singleton().p_curl_multi_init();
  if (!m_hnd) throw std::runtime_error("cURL error");
}

inline rowset::~rowset()
{
  for (auto row: m_pg)
  {
    lib::singleton().p_curl_multi_remove_handle(m_hnd, row.first);
    lib::singleton().p_curl_easy_cleanup(row.first);
    delete row.second.rast;
  }
  lib::singleton().p_curl_multi_cleanup(m_hnd);
}

inline std::vector<std::string> rowset::columns()
{
  std::vector<std::string> cols;
  for (size_t i(0); i < m_cols.size(); ++i)
  {
    if (m_cols[i]) cols.push_back(ColumnNamePng);
    else cols.push_back(ColumnNameWkb);
  }
  return cols;
}

inline void rowset::add_files()
{
  for (size_t i(0), count(PageSize - m_pg.size()); i < count; ++i)
  {
    if (m_rows == 0) break;
    tile tl(0, 0, 0);
    if (!m_tls.fetch(tl)) break;
    if (m_rows > 0) --m_rows;

    CURL* hnd(lib::singleton().p_curl_easy_init());
    if (!hnd) throw std::runtime_error("cURL error");
    auto del = [](CURL* ptr) { lib::singleton().p_curl_easy_cleanup(ptr); };
    std::unique_ptr<CURL, decltype(del)> raii_hnd(hnd, del);

    data_t data;
    data.tl = tl;
    data.rast = new blob_t();
    m_pg[hnd] = data;

    raii_hnd.release();
    const std::string url(m_lr->get_url(i, tl.x, tl.y, tl.z));
    check(lib::singleton().p_curl_easy_setopt(hnd, CURLOPT_URL, url.c_str()));
    check(lib::singleton().p_curl_easy_setopt(hnd, CURLOPT_WRITEFUNCTION, &write));
    check(lib::singleton().p_curl_easy_setopt(hnd, CURLOPT_WRITEDATA, data.rast));
    check(lib::singleton().p_curl_easy_setopt(hnd, CURLOPT_TIMEOUT_MS, long(TimeoutSec * 1000)));
    check(lib::singleton().p_curl_multi_add_handle(m_hnd, hnd));
  }
}

inline bool rowset::fetch(std::vector<variant>& row)
{
  if (m_pg.size() < (PageSize / 4))
    add_files();

  if (m_pg.empty())
    return false;

  while (true)
  {
    int still_running(0);
    while (lib::singleton().p_curl_multi_perform(m_hnd, &still_running) == CURLM_CALL_MULTI_PERFORM);
    if (still_running != int(m_pg.size())) break;
    add_files();
  }

  int msgs_in_queue(0);
  CURLMsg* msg(lib::singleton().p_curl_multi_info_read(m_hnd, &msgs_in_queue));
  if (!msg || msg->msg != CURLMSG_DONE) throw std::runtime_error("cURL error");
  check(msg->data.result);
  CURL* hnd(msg->easy_handle);

  const size_t count(m_cols.size());
  row.resize(count);
  data_t& data(m_pg.at(hnd));
  for (size_t i(0); i < count; ++i)
  {
    if (m_cols[i])
    {
      row[i] = blob_t();
      blob_t& blob = ::boost::get<blob_t>(row[i]);
      blob.assign(begin(*data.rast), end(*data.rast));
    }
    else
      row[i] = brig::boost::as_binary(data.tl.get_box());
  }

  lib::singleton().p_curl_multi_remove_handle(m_hnd, hnd);
  lib::singleton().p_curl_easy_cleanup(hnd);
  delete data.rast;
  m_pg.erase(hnd);

  return true;
} // rowset::

} } } // brig::osm::detail

#endif // BRIG_OSM_DETAIL_ROWSET_HPP
