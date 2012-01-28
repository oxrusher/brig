// Andrew Naplavkov

#ifndef BRIG_DATABASE_TYPE_IDENTIFIER_HPP
#define BRIG_DATABASE_TYPE_IDENTIFIER_HPP

namespace brig { namespace database {

enum TypeIdentifier {
  UnknownType,
  Date,
  DateTime,
  Double,
  Geometry,
  Integer,
  String
}; // TypeIdentifier

} } // brig::database

#endif // BRIG_DATABASE_TYPE_IDENTIFIER_HPP
