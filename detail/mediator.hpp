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

  template <typename Function, typename Result>
  class functor_impl : public functor {
    Function& m_func;
  public:
    Result m_res;
    functor_impl(Function& func) : m_func(func), m_res()  {}
    virtual void operator()(ThreadedArgument* arg)  { m_res = m_func(arg); }
  }; // functor_impl<Function, Result>

  template <typename Function>
  class functor_impl<Function, void> : public functor {
    Function& m_func;
  public:
    functor_impl(Function& func) : m_func(func)  {}
    virtual void operator()(ThreadedArgument* arg)  { m_func(arg); }
  }; // functor_impl<Function>

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
