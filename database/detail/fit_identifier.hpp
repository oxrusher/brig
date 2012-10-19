// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_FIT_IDENTIFIER_HPP
#define BRIG_DATABASE_DETAIL_FIT_IDENTIFIER_HPP

#include <algorithm>
#include <brig/database/global.hpp>
#include <brig/database/identifier.hpp>
#include <brig/unicode/lower_case.hpp>
#include <brig/unicode/transform.hpp>
#include <brig/unicode/upper_case.hpp>
#include <string>

namespace brig { namespace database { namespace detail {

inline std::string fit_identifier(const std::string& id_from, DBMS sys_to)
{
  using namespace std;
  using namespace brig::unicode;

  switch (sys_to)
  {
  default: return id_from;
  case Ingres:
  case MySQL:  return transform<string>(id_from, lower_case);
  case Oracle:
    {
    // The TABLE_NAME and COLUMN_NAME values are always converted to uppercase when you insert them into the USER_SDO_GEOM_METADATA view
    u32string u32(transform<u32string>(id_from, upper_case));
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
    return transform<string>(u32);
    }
  }
}

inline identifier fit_identifier(const identifier& id_from, DBMS sys_to, const std::string& schema_to)
{
  identifier id_to;
  id_to.schema = schema_to;
  id_to.name = fit_identifier(id_from.name, sys_to);
  id_to.qualifier = fit_identifier(id_from.qualifier, sys_to);
  return id_to;
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_FIT_IDENTIFIER_HPP
