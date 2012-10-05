// Andrew Naplavkov

#ifndef BRIG_DATABASE_SQLITE_DETAIL_LIB_HPP
#define BRIG_DATABASE_SQLITE_DETAIL_LIB_HPP

#include <brig/detail/dynamic_loading.hpp>
#include <sqlite3.h>

namespace brig { namespace database { namespace sqlite { namespace detail {

int sqlite3_extension_init(sqlite3 *db, const char **pzErrMsg, const struct sqlite3_api_routines *pThunk);
char* spatialite_version(void);

class lib {
  lib();

public:
  decltype(sqlite3_auto_extension) *p_sqlite3_auto_extension;
  decltype(sqlite3_bind_blob) *p_sqlite3_bind_blob;
  decltype(sqlite3_bind_double) *p_sqlite3_bind_double;
  decltype(sqlite3_bind_int64) *p_sqlite3_bind_int64;
  decltype(sqlite3_bind_null) *p_sqlite3_bind_null;
  decltype(sqlite3_bind_text) *p_sqlite3_bind_text;
  decltype(sqlite3_changes) *p_sqlite3_changes;
  decltype(sqlite3_close) *p_sqlite3_close;
  decltype(sqlite3_column_blob) *p_sqlite3_column_blob;
  decltype(sqlite3_column_bytes) *p_sqlite3_column_bytes;
  decltype(sqlite3_column_count) *p_sqlite3_column_count;
  decltype(sqlite3_column_decltype) *p_sqlite3_column_decltype;
  decltype(sqlite3_column_double) *p_sqlite3_column_double;
  decltype(sqlite3_column_int64) *p_sqlite3_column_int64;
  decltype(sqlite3_column_name) *p_sqlite3_column_name;
  decltype(sqlite3_column_text) *p_sqlite3_column_text;
  decltype(sqlite3_column_type) *p_sqlite3_column_type;
  decltype(sqlite3_errmsg) *p_sqlite3_errmsg;
  decltype(sqlite3_exec) *p_sqlite3_exec;
  decltype(sqlite3_finalize) *p_sqlite3_finalize;
  decltype(sqlite3_libversion) *p_sqlite3_libversion;
  decltype(sqlite3_open) *p_sqlite3_open;
  decltype(sqlite3_prepare_v2) *p_sqlite3_prepare_v2;
  decltype(sqlite3_reset) *p_sqlite3_reset;
  decltype(sqlite3_step) *p_sqlite3_step;

  decltype(spatialite_version) *p_spatialite_version;

  bool empty() const  { return p_sqlite3_step == 0; }
  static lib& singleton()  { static lib s; return s; }
}; // lib

inline lib::lib() : p_sqlite3_step(0), p_spatialite_version(0)
{
  // SQLite
  auto handle = BRIG_DL_LIBRARY("libsqlite3-0.dll", "libsqlite3.so.0");
  if (  handle
    && (p_sqlite3_auto_extension = BRIG_DL_FUNCTION(handle, sqlite3_auto_extension))
    && (p_sqlite3_bind_blob = BRIG_DL_FUNCTION(handle, sqlite3_bind_blob))
    && (p_sqlite3_bind_double = BRIG_DL_FUNCTION(handle, sqlite3_bind_double))
    && (p_sqlite3_bind_int64 = BRIG_DL_FUNCTION(handle, sqlite3_bind_int64))
    && (p_sqlite3_bind_null = BRIG_DL_FUNCTION(handle, sqlite3_bind_null))
    && (p_sqlite3_bind_text = BRIG_DL_FUNCTION(handle, sqlite3_bind_text))
    && (p_sqlite3_changes = BRIG_DL_FUNCTION(handle, sqlite3_changes))
    && (p_sqlite3_close = BRIG_DL_FUNCTION(handle, sqlite3_close))
    && (p_sqlite3_column_blob = BRIG_DL_FUNCTION(handle, sqlite3_column_blob))
    && (p_sqlite3_column_bytes = BRIG_DL_FUNCTION(handle, sqlite3_column_bytes))
    && (p_sqlite3_column_count = BRIG_DL_FUNCTION(handle, sqlite3_column_count))
    && (p_sqlite3_column_decltype = BRIG_DL_FUNCTION(handle, sqlite3_column_decltype))
    && (p_sqlite3_column_double = BRIG_DL_FUNCTION(handle, sqlite3_column_double))
    && (p_sqlite3_column_int64 = BRIG_DL_FUNCTION(handle, sqlite3_column_int64))
    && (p_sqlite3_column_name = BRIG_DL_FUNCTION(handle, sqlite3_column_name))
    && (p_sqlite3_column_text = BRIG_DL_FUNCTION(handle, sqlite3_column_text))
    && (p_sqlite3_column_type = BRIG_DL_FUNCTION(handle, sqlite3_column_type))
    && (p_sqlite3_errmsg = BRIG_DL_FUNCTION(handle, sqlite3_errmsg))
    && (p_sqlite3_exec = BRIG_DL_FUNCTION(handle, sqlite3_exec))
    && (p_sqlite3_finalize = BRIG_DL_FUNCTION(handle, sqlite3_finalize))
    && (p_sqlite3_libversion = BRIG_DL_FUNCTION(handle, sqlite3_libversion))
    && (p_sqlite3_open = BRIG_DL_FUNCTION(handle, sqlite3_open))
    && (p_sqlite3_prepare_v2 = BRIG_DL_FUNCTION(handle, sqlite3_prepare_v2))
    && (p_sqlite3_reset = BRIG_DL_FUNCTION(handle, sqlite3_reset))
     )  p_sqlite3_step = BRIG_DL_FUNCTION(handle, sqlite3_step);

   // SpatiaLite (optional)
   if (!empty())
   {
     handle = BRIG_DL_LIBRARY("libspatialite-2.dll", "libspatialite.so.2");
     if (handle)
     {
       p_spatialite_version = BRIG_DL_FUNCTION(handle, spatialite_version);
       decltype(sqlite3_extension_init) *p_sqlite3_extension_init = BRIG_DL_FUNCTION(handle, sqlite3_extension_init);
       if (p_sqlite3_extension_init) p_sqlite3_auto_extension((void(*)(void))p_sqlite3_extension_init);
     }
   }
} // lib::

} } } } // brig::database::sqlite::detail

#endif // BRIG_DATABASE_SQLITE_DETAIL_LIB_HPP
