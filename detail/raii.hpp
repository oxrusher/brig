// Andrew Naplavkov

// http://en.wikipedia.org/wiki/Resource_Acquisition_Is_Initialization

#ifndef BRIG_DETAIL_RAII_HPP
#define BRIG_DETAIL_RAII_HPP

#include <boost/utility.hpp>
#include <memory>

namespace brig { namespace detail {

template <typename Handle, typename Close>
class raii : ::boost::noncopyable {
  Handle m_handle;
  Close m_close;
public:
  raii(Handle handle, Close close) : m_handle(handle), m_close(close)  {}
  ~raii()  { if (m_handle) m_close(m_handle); }
  operator Handle() const  { return m_handle; }
  operator bool() const  { return m_handle != 0; }
  void reset()  { m_handle = 0; }
}; // raii

template <typename Handle, typename Close>
raii<Handle, Close> make_raii(Handle handle, Close close)
{
  return raii<Handle, Close>(handle, close);
}

} } // brig::detail

#endif // BRIG_DETAIL_RAII_HPP
