// Andrew Naplavkov

// todo: MSVC November 2012 CTP - variadic template problem

#ifndef BRIG_DETAIL_SIGNATURE_HPP
#define BRIG_DETAIL_SIGNATURE_HPP

namespace brig { namespace detail {

  template <typename... T>
  struct signature;

  template <typename R>
  struct signature<R()>
  {
    static const bool stdcall = false;
    static const size_t size = 0;
  };

  template <typename R, typename A1>
  struct signature<R(A1)>
  {
    static const bool stdcall = false;
    static const size_t size = sizeof(A1);
  };

  template <typename R, typename A1, typename A2>
  struct signature<R(A1, A2)>
  {
    static const bool stdcall = false;
    static const size_t size = sizeof(A1) + signature<R(A2)>::size;
  };

  template <typename R, typename A1, typename A2, typename A3>
  struct signature<R(A1, A2, A3)>
  {
    static const bool stdcall = false;
    static const size_t size = sizeof(A1) + signature<R(A2, A3)>::size;
  };

  template <typename R, typename A1, typename A2, typename A3, typename A4>
  struct signature<R(A1, A2, A3, A4)>
  {
    static const bool stdcall = false;
    static const size_t size = sizeof(A1) + signature<R(A2, A3, A4)>::size;
  };

  template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
  struct signature<R(A1, A2, A3, A4, A5)>
  {
    static const bool stdcall = false;
    static const size_t size = sizeof(A1) + signature<R(A2, A3, A4, A5)>::size;
  };

#ifdef _WIN32

  template <typename R>
  struct signature<R __stdcall ()>
  {
    static const bool stdcall = true;
    static const size_t size = 0;
  };

  template <typename R, typename A1>
  struct signature<R __stdcall (A1)>
  {
    static const bool stdcall = true;
    static const size_t size = sizeof(A1);
  };

  template <typename R, typename A1, typename A2>
  struct signature<R __stdcall (A1, A2)>
  {
    static const bool stdcall = true;
    static const size_t size = sizeof(A1) + signature<R __stdcall (A2)>::size;
  };

  template <typename R, typename A1, typename A2, typename A3>
  struct signature<R __stdcall (A1, A2, A3)>
  {
    static const bool stdcall = true;
    static const size_t size = sizeof(A1) + signature<R __stdcall (A2, A3)>::size;
  };

  template <typename R, typename A1, typename A2, typename A3, typename A4>
  struct signature<R __stdcall (A1, A2, A3, A4)>
  {
    static const bool stdcall = true;
    static const size_t size = sizeof(A1) + signature<R __stdcall (A2, A3, A4)>::size;
  };

  template <typename R, typename A1, typename A2, typename A3, typename A4, typename A5>
  struct signature<R __stdcall (A1, A2, A3, A4, A5)>
  {
    static const bool stdcall = true;
    static const size_t size = sizeof(A1) + signature<R __stdcall (A2, A3, A4, A5)>::size;
  };

#endif

} } // brig::detail

#endif // BRIG_DETAIL_SIGNATURE_HPP
