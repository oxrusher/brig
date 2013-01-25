// Andrew Naplavkov

#ifndef BRIG_INDEX_DEF_HPP
#define BRIG_INDEX_DEF_HPP

#include <brig/global.hpp>
#include <brig/identifier.hpp>
#include <string>
#include <vector>

namespace brig {

struct index_def {
  identifier id;
  index_type type;
  std::vector<std::string> columns;

  index_def() : type(VoidIndex)  {}
}; // index_def

} // brig

#endif // BRIG_INDEX_DEF_HPP
