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
class mediator : boost::noncopyable {
public:
  struct functor {
    virtual ~functor()  {}
    virtual void operator()(ThreadedArgument*) = 0;
  }; // functor

  template <typename R, typename F>
  struct functor_impl : functor {
    R r;
    F f;
    explicit functor_impl(F&& f_) : f(std::move(f_))  {}
    virtual void operator()(ThreadedArgument* arg)  { r = f(arg); }
  }; // functor_impl<R, F>

  template <typename F>
  struct functor_impl<void, F> : functor {
    F f;
    explicit functor_impl(F&& f_) : f(std::move(f_))  {}
    virtual void operator()(ThreadedArgument* arg)  { f(arg); }
  }; // functor_impl<void, F>

  template <typename F>
  static functor_impl<typename F::result_type, F> bind(F&& f)  { return std::move(functor_impl<typename F::result_type, F>(std::move(f))); }

private:
  enum state  { BeforeStart, Idle, Calling, AfterFinish };

  boost::mutex m_mut;
  boost::condition_variable m_cond;
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
    boost::unique_lock<boost::mutex> lock(m_mut);
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
    boost::unique_lock<boost::mutex> lock(m_mut);
    m_cond.wait(lock, [&](){ return Idle == this->m_st || AfterFinish == this->m_st; });
    if (Idle == m_st)
    {
      m_st = Calling;
      m_func = func;
      m_exc = std::exception_ptr();
    }
  }
  m_cond.notify_one();

  boost::unique_lock<boost::mutex> lock(m_mut);
  m_cond.wait(lock, [&](){ return Idle == this->m_st || AfterFinish == this->m_st; });
  m_func = 0;
  if (!(m_exc == 0)) std::rethrow_exception(std::move(m_exc));
  if (AfterFinish == m_st) throw std::runtime_error("thread is over");
}

template <typename ThreadedArgument>
bool mediator<ThreadedArgument>::handle(ThreadedArgument* arg)
{
  {
    boost::unique_lock<boost::mutex> lock(m_mut);
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
