// Andrew Naplavkov

#ifndef BRIG_OSM_DETAIL_CURL_HPP
#define BRIG_OSM_DETAIL_CURL_HPP

#include <curl/curl.h>
#include <brig/detail/dynamic_loading.hpp>

namespace brig { namespace osm { namespace detail {

class lib {
  lib();

public:
  decltype(curl_easy_cleanup) *p_curl_easy_cleanup;
  decltype(curl_easy_init) *p_curl_easy_init;
  decltype(curl_easy_setopt) *p_curl_easy_setopt;
  decltype(curl_easy_strerror) *p_curl_easy_strerror;
  decltype(curl_multi_add_handle) *p_curl_multi_add_handle;
  decltype(curl_multi_cleanup) *p_curl_multi_cleanup;
  decltype(curl_multi_info_read) *p_curl_multi_info_read;
  decltype(curl_multi_init) *p_curl_multi_init;
  decltype(curl_multi_perform) *p_curl_multi_perform;
  decltype(curl_multi_remove_handle) *p_curl_multi_remove_handle;
  decltype(curl_multi_strerror) *p_curl_multi_strerror;
  decltype(curl_version) *p_curl_version;

  bool empty() const  { return p_curl_version == 0; }
  static lib& singleton()  { static lib s; return s; }
}; // lib

inline lib::lib() : p_curl_version(0)
{
  auto handle(BRIG_DL_LIBRARY("libcurl.dll", "")); // todo:
  if (  handle
  && (p_curl_easy_cleanup = BRIG_DL_FUNCTION(handle, curl_easy_cleanup))
  && (p_curl_easy_init = BRIG_DL_FUNCTION(handle, curl_easy_init))
  && (p_curl_easy_setopt = BRIG_DL_FUNCTION(handle, curl_easy_setopt))
  && (p_curl_easy_strerror = BRIG_DL_FUNCTION(handle, curl_easy_strerror))
  && (p_curl_multi_add_handle = BRIG_DL_FUNCTION(handle, curl_multi_add_handle))
  && (p_curl_multi_cleanup = BRIG_DL_FUNCTION(handle, curl_multi_cleanup))
  && (p_curl_multi_info_read = BRIG_DL_FUNCTION(handle, curl_multi_info_read))
  && (p_curl_multi_init = BRIG_DL_FUNCTION(handle, curl_multi_init))
  && (p_curl_multi_perform = BRIG_DL_FUNCTION(handle, curl_multi_perform))
  && (p_curl_multi_remove_handle = BRIG_DL_FUNCTION(handle, curl_multi_remove_handle))
  && (p_curl_multi_strerror = BRIG_DL_FUNCTION(handle, curl_multi_strerror))
   )  p_curl_version = BRIG_DL_FUNCTION(handle, curl_version);
} // lib::

} } } // brig::osm::detail

#endif // BRIG_OSM_DETAIL_CURL_HPP
