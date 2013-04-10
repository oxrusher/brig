// Andrew Naplavkov

#ifndef BRIG_GLOBAL_HPP
#define BRIG_GLOBAL_HPP

#include <string>

namespace brig {

enum column_type {
  VoidColumn,
  Blob,
  Double,
  Geometry,
  Integer,
  String
}; // column_type

enum index_type {
  VoidIndex,
  Primary,
  Unique,
  Duplicate,
  Spatial
}; // index_type

const int CharsLimit = 250;
const size_t PageSize = 250; // DB2 PUERTO_ROADS is slowdown after 447
const size_t PoolSize = 8;
const size_t TimeoutSec = 120;

const char TBL[] = "tbl";
const char WKB[] = "wkb";
const char PNG[] = "png";

} // brig

#endif // BRIG_GLOBAL_HPP
