// Andrew Naplavkov

#ifndef BRIG_DATABASE_CUBRID_DETAIL_LIB_HPP
#define BRIG_DATABASE_CUBRID_DETAIL_LIB_HPP

#include <brig/detail/dynamic_loading.hpp>
#include <cas_cci.h>

namespace brig { namespace database { namespace cubrid { namespace detail {

class lib {
  lib();

public:
  decltype(cci_bind_param) *p_cci_bind_param;
  decltype(cci_close_req_handle) *p_cci_close_req_handle;
  decltype(cci_connect_with_url) *p_cci_connect_with_url;
  decltype(cci_cursor) *p_cci_cursor;
  decltype(cci_disconnect) *p_cci_disconnect;
  decltype(cci_end_tran) *p_cci_end_tran;
  decltype(cci_execute) *p_cci_execute;
  decltype(cci_fetch) *p_cci_fetch;
  decltype(cci_get_autocommit) *p_cci_get_autocommit;
  decltype(cci_get_data) *p_cci_get_data;
  decltype(cci_get_result_info) *p_cci_get_result_info;
  decltype(cci_prepare) *p_cci_prepare;
  decltype(cci_set_autocommit) *p_cci_set_autocommit;

  bool empty() const  { return p_cci_set_autocommit == 0; }
  static lib& singleton()  { static lib s; return s; }
  static bool error(int r)  { return r < CCI_ER_NO_ERROR; }
}; // lib

inline lib::lib() : p_cci_set_autocommit(0)
{
  auto handle = BRIG_DL_LIBRARY("cascci.dll", "libcascci.so");
  if (  handle
    && (p_cci_bind_param = BRIG_DL_FUNCTION(handle, cci_bind_param))
    && (p_cci_close_req_handle = BRIG_DL_FUNCTION(handle, cci_close_req_handle))
    && (p_cci_connect_with_url = BRIG_DL_FUNCTION(handle, cci_connect_with_url))
    && (p_cci_cursor = BRIG_DL_FUNCTION(handle, cci_cursor))
    && (p_cci_disconnect = BRIG_DL_FUNCTION(handle, cci_disconnect))
    && (p_cci_end_tran = BRIG_DL_FUNCTION(handle, cci_end_tran))
    && (p_cci_execute = BRIG_DL_FUNCTION(handle, cci_execute))
    && (p_cci_fetch = BRIG_DL_FUNCTION(handle, cci_fetch))
    && (p_cci_get_autocommit = BRIG_DL_FUNCTION(handle, cci_get_autocommit))
    && (p_cci_get_data = BRIG_DL_FUNCTION(handle, cci_get_data))
    && (p_cci_get_result_info = BRIG_DL_FUNCTION(handle, cci_get_result_info))
    && (p_cci_prepare = BRIG_DL_FUNCTION(handle, cci_prepare))
     )  p_cci_set_autocommit = BRIG_DL_FUNCTION(handle, cci_set_autocommit);
} // lib::

} } } } // brig::database::cubrid::detail

#endif // BRIG_DATABASE_CUBRID_DETAIL_LIB_HPP
