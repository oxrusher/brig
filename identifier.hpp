// Andrew Naplavkov

#ifndef BRIG_IDENTIFIER_HPP
#define BRIG_IDENTIFIER_HPP

#include <string>

namespace brig {

struct identifier {
  std::string schema, name, qualifier;

  std::string to_string() const  { return schema.empty()? name: (schema + "." + name); }
}; // identifier

} // brig

#endif // BRIG_IDENTIFIER_HPP
