// Andrew Naplavkov

#ifndef BRIG_INDEX_DEFINITION_HPP
#define BRIG_INDEX_DEFINITION_HPP

#include <brig/global.hpp>
#include <brig/identifier.hpp>
#include <string>
#include <vector>

namespace brig {

struct index_definition {
  identifier id;
  index_type type;
  std::vector<std::string> columns;

  index_definition() : type(VoidIndex)  {}
}; // index_definition

} // brig

#endif // BRIG_INDEX_DEFINITION_HPP
