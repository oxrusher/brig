// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_NORMALIZE_IDENTIFIER_HPP
#define BRIG_DATABASE_DETAIL_NORMALIZE_IDENTIFIER_HPP

#include <brig/database/global.hpp>
#include <brig/database/identifier.hpp>
#include <brig/unicode/transform.hpp>
#include <brig/unicode/upper_case.hpp>
#include <string>

namespace brig { namespace database { namespace detail {

inline void normalize_identifier(DBMS sys, std::string& id)
{
  using namespace brig::unicode;
  if (Oracle == sys) // The TABLE_NAME and COLUMN_NAME values are always converted to uppercase when you insert them into the USER_SDO_GEOM_METADATA view
    id = transform<std::string>(id, upper_case);
}

inline void normalize_identifier(DBMS sys, identifier& id)
{
  using namespace brig::unicode;
  if (Oracle == sys) // The TABLE_NAME and COLUMN_NAME values are always converted to uppercase when you insert them into the USER_SDO_GEOM_METADATA view
  {
    id.name = transform<std::string>(id.name, upper_case);
    id.qualifier = transform<std::string>(id.qualifier, upper_case);
  }
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_NORMALIZE_IDENTIFIER_HPP
