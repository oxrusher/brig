// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_TO_LCASE_HPP
#define BRIG_DATABASE_DETAIL_TO_LCASE_HPP

#include <brig/unicode/lower_case.hpp>
#include <brig/unicode/transform.hpp>
#include <string>

namespace brig { namespace database { namespace detail {

template <typename InputCodeUnit>
std::string to_lcase(const InputCodeUnit* ptr)
{
  using namespace std;
  using namespace brig::unicode;
  return transform<string>(ptr, lower_case);
}

template <typename InputCodeUnit>
std::string to_lcase(const std::basic_string<InputCodeUnit>& str)
{
  using namespace std;
  using namespace brig::unicode;
  return transform<string>(str, lower_case);
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_TO_LCASE_HPP
