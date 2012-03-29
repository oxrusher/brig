// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_NORMALIZE_IDENTIFIER_HPP
#define BRIG_DATABASE_DETAIL_NORMALIZE_IDENTIFIER_HPP

#include <algorithm>
#include <brig/database/global.hpp>
#include <brig/database/identifier.hpp>
#include <brig/unicode/transform.hpp>
#include <brig/unicode/upper_case.hpp>
#include <string>

namespace brig { namespace database { namespace detail {

inline void normalize_identifier(DBMS sys, std::string& id)
{
  using namespace brig::unicode;
  if (Oracle != sys) return;
  std::u32string id_(transform<std::u32string>(id, upper_case)); // The TABLE_NAME and COLUMN_NAME values are always converted to uppercase when you insert them into the USER_SDO_GEOM_METADATA view
  std::transform(std::begin(id_), std::end(id_), std::begin(id_), [](char32_t ch) -> char32_t {
    switch (ch)
    {
    default:
      return ch;
    case 0x20: // space
    case 0x2d: // minus
      return 0x5f; // underline
    }
  });
  id = transform<std::string>(id_);
}

inline void normalize_identifier(DBMS sys, identifier& id)
{
  normalize_identifier(sys, id.name);
  normalize_identifier(sys, id.qualifier);
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_NORMALIZE_IDENTIFIER_HPP
