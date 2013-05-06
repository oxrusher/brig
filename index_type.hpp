// Andrew Naplavkov

#ifndef BRIG_INDEX_TYPE_HPP
#define BRIG_INDEX_TYPE_HPP

namespace brig {

enum class index_type {
  Void,
  Primary,
  Unique,
  Duplicate,
  Spatial
}; // index_type

} // brig

#endif // BRIG_INDEX_TYPE_HPP
