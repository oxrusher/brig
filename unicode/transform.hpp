// Andrew Naplavkov

#ifndef BRIG_UNICODE_TRANSFORM_HPP
#define BRIG_UNICODE_TRANSFORM_HPP

#include <brig/detail/back_insert_iterator.hpp>
#include <brig/unicode/detail/utf.hpp>
#include <cstdint>
#include <string>

namespace brig { namespace unicode {

template <typename InputIterator, typename OutputIterator>
void transform(InputIterator& in_itr, OutputIterator& out_itr)
{
  while (true)
  {
    const uint32_t cp(detail::utf<InputIterator>::type::get_code_point(in_itr));
    if (cp == 0) break;
    detail::utf<OutputIterator>::type::set_code_point(out_itr, cp);
  }
}

template <typename OutputString, typename InputCodeUnit>
OutputString transform(const InputCodeUnit* in_ptr)
{
  OutputString out_str;
  if (in_ptr != 0)
  {
    brig::detail::back_insert_iterator<OutputString> out_itr(out_str);
    transform(in_ptr, out_itr);
  }
  return std::move(out_str);
}

template <typename OutputString, typename InputCodeUnit>
OutputString transform(const std::basic_string<InputCodeUnit>& in_str)
{
  OutputString out_str;
  brig::detail::back_insert_iterator<OutputString> out_itr(out_str);
  auto in_ptr = in_str.c_str();
  transform(in_ptr, out_itr);
  return std::move(out_str);
}

} } // brig::unicode

#endif // BRIG_UNICODE_TRANSFORM_HPP
