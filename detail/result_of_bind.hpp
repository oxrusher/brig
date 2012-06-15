// Andrew Naplavkov

#ifndef BRIG_DETAIL_RESULT_OF_BIND_HPP
#define BRIG_DETAIL_RESULT_OF_BIND_HPP

#include <functional>
#include <utility>

namespace brig { namespace detail {

template <typename F, typename... Args>
struct result_of_bind {
  typedef decltype(std::bind(std::declval<F>(), std::declval<Args>()...)) type;
  typedef typename type::result_type result_type;
}; // result_of_bind

} } // brig::detail

#endif // BRIG_DETAIL_RESULT_OF_BIND_HPP
