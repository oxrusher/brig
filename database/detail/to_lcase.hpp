// Andrew Naplavkov

#ifndef BRIG_DATABASE_DETAIL_TO_LCASE_HPP
#define BRIG_DATABASE_DETAIL_TO_LCASE_HPP

#include <brig/unicode/lower_case.hpp>
#include <brig/unicode/transform.hpp>
#include <string>

namespace brig { namespace database { namespace detail {

template <typename Text>
std::string to_lcase(Text text)
{
  using namespace std;
  using namespace brig::unicode;
  return transform<string>(text, lower_case);
}

} } } // brig::database::detail

#endif // BRIG_DATABASE_DETAIL_TO_LCASE_HPP
