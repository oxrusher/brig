// Andrew Naplavkov

#ifndef BRIG_DETAIL_MEDIATOR_HPP
#define BRIG_DETAIL_MEDIATOR_HPP

#include <boost/utility.hpp>
#include <brig/detail/result_of_bind.hpp>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <stdexcept>
#include <utility>

namespace brig { namespace detail {

template <typename ThreadedArgument>
class mediator : ::boost::noncopyable {
  struct task {
    virtual void exec(ThreadedArgument*) = 0;
  }; // task

  template <typename R, typename F>
  struct task_impl : task {
    R r;
    F f;
    explicit task_impl(F&& f_) : f(std::forward<F>(f_))  {}
    virtual void exec(ThreadedArgument* arg)  { r = f(arg); }
    R result()  { return std::move(r); }
  }; // task_impl<R, F>

  template <typename F>
  struct task_impl<void, F> : task {
    F f;
    explicit task_impl(F&& f_) : f(std::forward<F>(f_))  {}
    virtual void exec(ThreadedArgument* arg)  { f(arg); }
    void result()  {}
  }; // task_impl<void, F>

  enum state  { BeforeStart, Idle, Calling, AfterFinish };

  std::mutex m_mut;
  std::condition_variable m_cond;
  state m_st;
  task* m_tsk;
  std::exception_ptr m_exc;

  void set_state(state, const std::exception_ptr&);
  void exec(task*); // rethrow exception

public:
  mediator() : m_st(BeforeStart), m_tsk(0)  {}
  void start()  { set_state(Idle, std::exception_ptr()); }
  void stop(const std::exception_ptr& exc = std::exception_ptr())  { set_state(AfterFinish, exc); }
  template<typename F, typename... Args> auto call(F&& f, Args&&... args) -> typename result_of_bind<F, Args...>::result_type;
  bool handle(ThreadedArgument*); // catch exception
}; // mediator

template <typename ThreadedArgument>
void mediator<ThreadedArgument>::set_state(state st, const std::exception_ptr& exc)
{
  {
    std::unique_lock<std::mutex> lock(m_mut);
    m_st = st;
    m_tsk = 0;
    m_exc = exc;
  }
  m_cond.notify_one();
}

template <typename ThreadedArgument>
void mediator<ThreadedArgument>::exec(task* tsk)
{
  {
    std::unique_lock<std::mutex> lock(m_mut);
    m_cond.wait(lock, [&](){ return Idle == this->m_st || AfterFinish == this->m_st; });
    if (Idle == m_st)
    {
      m_st = Calling;
      m_tsk = tsk;
      m_exc = std::exception_ptr();
    }
  }
  m_cond.notify_one();

  std::unique_lock<std::mutex> lock(m_mut);
  m_cond.wait(lock, [&](){ return Idle == this->m_st || AfterFinish == this->m_st; });
  m_tsk = 0;
  if (!(m_exc == 0)) std::rethrow_exception(std::move(m_exc));
  if (AfterFinish == m_st) throw std::runtime_error("thread error");
}

template <typename ThreadedArgument>
  template<typename F, typename... Args>
auto mediator<ThreadedArgument>::call(F&& f, Args&&... args) -> typename result_of_bind<F, Args...>::result_type
{
  typedef typename result_of_bind<F, Args...>::result_type R;
  task_impl<R, std::function<R(ThreadedArgument*)>> tsk(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
  exec(&tsk);
  return tsk.result();
}

template <typename ThreadedArgument>
bool mediator<ThreadedArgument>::handle(ThreadedArgument* arg)
{
  {
    std::unique_lock<std::mutex> lock(m_mut);
    m_cond.wait(lock, [&](){ return Calling == this->m_st || AfterFinish == this->m_st; });
    if (AfterFinish == m_st) return false;

    m_st = Idle;
    try  { m_tsk->exec(arg); }
    catch (const std::exception&)  { m_exc = std::current_exception(); }
  }
  m_cond.notify_one();
  return true;
} // mediator::

} } // brig::detail

#endif // BRIG_DETAIL_MEDIATOR_HPP
