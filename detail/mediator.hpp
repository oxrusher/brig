// Andrew Naplavkov

#ifndef BRIG_DETAIL_MEDIATOR_HPP
#define BRIG_DETAIL_MEDIATOR_HPP

#include <boost/thread/condition_variable.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/utility.hpp>
#include <stdexcept>

namespace brig { namespace detail {

template <typename ThreadedArgument>
class mediator : ::boost::noncopyable {
public:
  struct functor {
    virtual ~functor()  {}
    virtual void operator()(ThreadedArgument*) = 0;
  }; // functor

  template <typename R, typename Func>
  struct functor_impl : functor {
    R r;
    Func func;
    explicit functor_impl(Func&& func_) : func(std::forward<Func>(func_))  {}
    virtual void operator()(ThreadedArgument* arg)  { r = func(arg); }
  }; // functor_impl<R, Func>

  template <typename Func>
  struct functor_impl<void, Func> : functor {
    Func func;
    explicit functor_impl(Func&& func_) : func(std::forward<Func>(func_))  {}
    virtual void operator()(ThreadedArgument* arg)  { func(arg); }
  }; // functor_impl<void, Func>

  template <typename Func>
  static functor_impl<typename Func::result_type, Func> bind(Func&& func)  { return functor_impl<typename Func::result_type, Func>(std::forward<Func>(func)); }

private:
  enum state  { BeforeStart, Idle, Calling, AfterFinish };

  ::boost::mutex m_mut;
  ::boost::condition_variable m_cond;
  state m_st;
  functor* m_func;
  std::exception_ptr m_exc;

  void set_state(state, const std::exception_ptr&);

public:
  mediator() : m_st(BeforeStart), m_func(0)  {}
  void start()  { set_state(Idle, std::exception_ptr()); }
  void stop(const std::exception_ptr& exc = std::exception_ptr())  { set_state(AfterFinish, exc); }
  void call(functor*); // rethrow exception
  bool handle(ThreadedArgument*); // catch exception
}; // mediator

template <typename ThreadedArgument>
void mediator<ThreadedArgument>::set_state(state st, const std::exception_ptr& exc)
{
  {
    ::boost::unique_lock<typename ::boost::mutex> lock(m_mut);
    m_st = st;
    m_func = 0;
    m_exc = exc;
  }
  m_cond.notify_one();
}

template <typename ThreadedArgument>
void mediator<ThreadedArgument>::call(functor* func)
{
  {
    ::boost::unique_lock<typename ::boost::mutex> lock(m_mut);
    m_cond.wait(lock, [&](){ return Idle == this->m_st || AfterFinish == this->m_st; });
    if (Idle == m_st)
    {
      m_st = Calling;
      m_func = func;
      m_exc = std::exception_ptr();
    }
  }
  m_cond.notify_one();

  ::boost::unique_lock<typename ::boost::mutex> lock(m_mut);
  m_cond.wait(lock, [&](){ return Idle == this->m_st || AfterFinish == this->m_st; });
  m_func = 0;
  if (!(m_exc == 0)) std::rethrow_exception(std::move(m_exc));
  if (AfterFinish == m_st) throw std::runtime_error("thread error");
}

template <typename ThreadedArgument>
bool mediator<ThreadedArgument>::handle(ThreadedArgument* arg)
{
  {
    ::boost::unique_lock<typename ::boost::mutex> lock(m_mut);
    m_cond.wait(lock, [&](){ return Calling == this->m_st || AfterFinish == this->m_st; });
    if (AfterFinish == m_st) return false;

    m_st = Idle;
    try  { (*m_func)(arg); }
    catch (const std::exception&)  { m_exc = std::current_exception(); }
  }
  m_cond.notify_one();
  return true;
} // mediator::

} } // brig::detail

#endif // BRIG_DETAIL_MEDIATOR_HPP
