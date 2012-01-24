// Andrew Naplavkov

#ifndef BRIG_UNICODE_TRANSFORM_HPP
#define BRIG_UNICODE_TRANSFORM_HPP

#include <brig/detail/back_insert_iterator.hpp>
#include <brig/unicode/detail/utf.hpp>
#include <cstdint>
#include <string>

namespace brig { namespace unicode { namespace detail {

template <typename InputIterator, typename OutputIterator>
void transform(InputIterator& in_iter, OutputIterator& out_iter)
{
  while (true)
  {
    const uint32_t cp(detail::utf<InputIterator>::type::get_code_point(in_iter));
    if (cp == 0) break;
    detail::utf<OutputIterator>::type::set_code_point(out_iter, cp);
  }
}

} // detail

template <typename OutputString, typename InputCodeUnit>
OutputString transform(const InputCodeUnit* ptr)
{
  OutputString res;
  if (ptr != 0)
  {
    auto out_iter = brig::detail::back_inserter(res);
    detail::transform(ptr, out_iter);
  }
  return std::move(res);
}

template <typename OutputString, typename InputCodeUnit>
OutputString transform(const std::basic_string<InputCodeUnit>& str)
{
  OutputString res;
  auto in_ptr = str.c_str();
  auto out_iter = brig::detail::back_inserter(res);
  detail::transform(in_ptr, out_iter);
  return std::move(res);
}

} } // brig::unicode

#endif // BRIG_UNICODE_TRANSFORM_HPP
