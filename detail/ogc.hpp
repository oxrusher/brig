// Andrew Naplavkov

// OpenGIS Document 99-049: http://www.opengeospatial.org/standards/sfs

#ifndef BRIG_DETAIL_OGC_HPP
#define BRIG_DETAIL_OGC_HPP

#include <brig/detail/copy.hpp>
#include <cstdint>
#include <iterator>
#include <stdexcept>

namespace brig { namespace detail { namespace ogc {

enum ByteOrder {
  BigEndian = 0,
  LittleEndian = 1
};

inline uint8_t system_byte_order()
{
  static const uint16_t two_bytes(1);
  static const uint8_t byte_order(*((const uint8_t*)&two_bytes) == 0? BigEndian : LittleEndian);
  return byte_order;
}

template <typename InputIterator>
uint8_t read_byte_order(InputIterator& iter)
{
  static_assert(sizeof(typename std::iterator_traits<InputIterator>::value_type) == sizeof(uint8_t), "size error");
  const uint8_t byte_order(static_cast<uint8_t>(*iter));
  ++iter;
  if (BigEndian == byte_order || LittleEndian == byte_order) return byte_order;
  throw std::runtime_error("byte order error");
}

template <typename OutputIterator>
void write_byte_order(OutputIterator& iter)
{
  static_assert(sizeof(typename std::iterator_traits<OutputIterator>::value_type) == sizeof(uint8_t), "size error");
  *iter = system_byte_order();
  ++iter;
}

template <typename T, typename InputIterator>
T read(uint8_t byte_order, InputIterator& iter)
{
  T val(0);
  uint8_t* ptr = (uint8_t*)(&val);
  if (system_byte_order() == byte_order)
    copy<T>(iter, ptr);
  else
    reverse_copy<T>(iter, ptr);
  return val;
}

template <typename T, typename OutputIterator>
void write(OutputIterator& iter, T val)
{
  const uint8_t* ptr = (const uint8_t*)(&val);
  copy<T>(ptr, iter);
}

enum GeometryType {
  Point = 1,
  LineString = 2,
  Polygon = 3,
  MultiPoint = 4,
  MultiLineString = 5,
  MultiPolygon = 6,
  GeometryCollection = 7
};

} } } // brig::detail::ogc

#endif // BRIG_DETAIL_OGC_HPP
