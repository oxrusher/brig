// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_NORMALIZE_IDENTIFIER_HPP
#define BRIG_DATABASE_DETAIL_NORMALIZE_IDENTIFIER_HPP

#include <algorithm>
#include <brig/database/global.hpp>
#include <brig/database/identifier.hpp>
#include <brig/unicode/lower_case.hpp>
#include <brig/unicode/transform.hpp>
#include <brig/unicode/upper_case.hpp>
#include <string>

namespace brig { namespace database { namespace detail {

inline void normalize_identifier(DBMS sys, std::string& id)
{
  using namespace std;
  using namespace brig::unicode;
  switch (sys)
  {
  default:
    break;

  case Ingres:
  case MySQL:
    id = transform<string>(id, lower_case);
    break;

  case Oracle:
    {
    u32string u32(transform<u32string>(id, upper_case)); // The TABLE_NAME and COLUMN_NAME values are always converted to uppercase when you insert them into the USER_SDO_GEOM_METADATA view
    transform(begin(u32), end(u32), begin(u32), [](char32_t ch) -> char32_t
    {
      switch (ch)
      {
      default:
        return ch;
      case 0x20: // space
      case 0x2d: // minus
        return 0x5f; // underline
      }
    });
    id = transform<string>(u32);
    }
    break;
  }
}

inline void normalize_identifier(DBMS sys, identifier& id)
{
  normalize_identifier(sys, id.name);
  if (Oracle == sys) normalize_identifier(sys, id.qualifier);
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_NORMALIZE_IDENTIFIER_HPP
