// Andrew Naplavkov

#ifndef BRIG_GLOBAL_HPP
#define BRIG_GLOBAL_HPP

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
const size_t PageSize = 500; // DB2 PUERTO_ROADS is slowdown after 447
const size_t PoolSize = 5;
const size_t Timeout = 30000; // ms

} // brig

#endif // BRIG_GLOBAL_HPP
