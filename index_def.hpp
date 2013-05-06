// Andrew Naplavkov

#ifndef BRIG_INDEX_DEF_HPP
#define BRIG_INDEX_DEF_HPP

#include <brig/index_type.hpp>
#include <brig/identifier.hpp>
#include <string>
#include <vector>

namespace brig {

struct index_def {
  identifier id;
  index_type type;
  std::vector<std::string> columns;

  index_def() : type(index_type::Void)  {}
}; // index_def

} // brig

#endif // BRIG_INDEX_DEF_HPP
