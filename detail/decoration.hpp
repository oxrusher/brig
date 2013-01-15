// Andrew Naplavkov

#ifndef BRIG_DETAIL_DECORATION_HPP
#define BRIG_DETAIL_DECORATION_HPP

#include <brig/detail/itoa.hpp>
#include <brig/detail/signature.hpp>
#include <brig/detail/stringify.hpp>
#include <string>

#define BRIG_DECORATION(fun) \
  brig::detail::signature<decltype(fun)>::stdcall \
? std::string(std::string("_" BRIG_STRINGIFY(fun) "@") + (const char*)brig::detail::itoa<brig::detail::signature<decltype(fun)>::size>::type::value).c_str() \
: BRIG_STRINGIFY(fun)

#endif // BRIG_DETAIL_DECORATION_HPP
