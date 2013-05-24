// Andrew Naplavkov

#ifndef BRIG_DETAIL_COPY_HPP
#define BRIG_DETAIL_COPY_HPP

#include <cstdint>
#include <iterator>

namespace brig { namespace detail {

template <typename T, typename InputIterator, typename OutputIterator>
void copy(InputIterator& in_itr, OutputIterator& out_itr)
{
  static_assert(sizeof(typename std::iterator_traits<InputIterator>::value_type) == sizeof(uint8_t), "size error");
  static_assert(sizeof(typename std::iterator_traits<OutputIterator>::value_type) == sizeof(uint8_t), "size error");

  for (size_t i(0); i < sizeof(T); ++i)
  {
    *out_itr = static_cast<uint8_t>(*in_itr);
    ++in_itr;
    ++out_itr;
  }
}

template <typename T, typename InputType, typename OutputType>
void copy(InputType*& in_ptr, OutputType*& out_ptr) // optimization
{
  static_assert(sizeof(InputType) == sizeof(uint8_t), "size error");
  static_assert(sizeof(OutputType) == sizeof(uint8_t), "size error");

  *(T*)(out_ptr) = *(const T*)(in_ptr);
  in_ptr += sizeof(T);
  out_ptr += sizeof(T);
}

template <typename T, typename InputIterator, typename OutputType>
void reverse_copy(InputIterator& in_itr, OutputType*& out_ptr)
{
  static_assert(sizeof(typename std::iterator_traits<InputIterator>::value_type) == sizeof(uint8_t), "size error");
  static_assert(sizeof(OutputType) == sizeof(uint8_t), "size error");

  for (size_t i(0); i < sizeof(T); ++i)
  {
    out_ptr[sizeof(T) - i - 1] = static_cast<uint8_t>(*in_itr);
    ++in_itr;
  }
  out_ptr += sizeof(T);
}

} } // brig::detail

#endif // BRIG_DETAIL_COPY_HPP
