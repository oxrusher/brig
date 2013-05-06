// Andrew Naplavkov

#ifndef BRIG_COLUMN_TYPE_HPP
#define BRIG_COLUMN_TYPE_HPP

namespace brig {

enum class column_type {
  Void,
  Blob,
  Double,
  Geometry,
  Integer,
  String
}; // column_type

} // brig

#endif // BRIG_COLUMN_TYPE_HPP
