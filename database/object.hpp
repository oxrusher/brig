// Andrew Naplavkov

#ifndef BRIG_DATABASE_OBJECT_HPP
#define BRIG_DATABASE_OBJECT_HPP

#include <ostream>
#include <string>

namespace brig { namespace database {

struct object {
  std::string schema, name;

  object()  {}
  object(std::string schema_, std::string name_) : schema(schema_), name(name_)  {}
}; // object

} } // brig::database

namespace std {

template <class CharT, class TraitsT>
basic_ostream<CharT, TraitsT>& operator<<(basic_ostream<CharT, TraitsT>& stream, const brig::database::object& obj)
{
  if (!obj.schema.empty()) stream << obj.schema << CharT(0x2e);
  stream << obj.name;
  return stream;
}

} // std

#endif // BRIG_DATABASE_OBJECT_HPP
