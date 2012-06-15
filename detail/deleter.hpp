// Andrew Naplavkov

#ifndef BRIG_DETAIL_DELETER_HPP
#define BRIG_DETAIL_DELETER_HPP

#include <memory>

namespace brig { namespace detail {

template <typename T, typename Allocator>
class deleter {
  std::shared_ptr<Allocator> m_allocator;
public:
  explicit deleter(std::shared_ptr<Allocator> allocator) : m_allocator(allocator)  {}
  void operator()(T* p) const  { m_allocator->deallocate(p); }
}; // deleter

} } // brig::detail

#endif // BRIG_DETAIL_DELETER_HPP
