// Andrew Naplavkov

#ifndef BRIG_DETAIL_MEDIATOR_HPP
#define BRIG_DETAIL_MEDIATOR_HPP

#include <boost/utility.hpp>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <stdexcept>
#include <utility>

namespace brig { namespace detail {

template <typename Interface>
class mediator : ::boost::noncopyable {
  struct task {
    virtual ~task()  {}
    virtual void exec(Interface* iface) = 0;
  }; // task

  template <typename Result, typename UnaryFun>
  struct task_impl : task {
    Result r;
    UnaryFun& f;
    explicit task_impl(UnaryFun& f_) : f(f_)  {}
    void exec(Interface* iface) override  { r = f(iface); }
    Result result()  { return std::move(r); }
  }; // task_impl<Result, UnaryFun>

  template <typename UnaryFun>
  struct task_impl<void, UnaryFun> : task {
    UnaryFun& f;
    explicit task_impl(UnaryFun& f_) : f(f_)  {}
    void exec(Interface* iface) override  { f(iface); }
    void result()  {}
  }; // task_impl<void, UnaryFun>

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
  template<typename Result, typename Fun, typename... Args>
  Result call(Fun&& f, Args&&... args);
  // todo: GCC - decltype(std::bind(std::forward<Fun>(f), std::forward<Args>(args)...)(std::declval<Interface*>()));
  // todo: MSVC November 2012 CTP - typename std::result_of<Fun(Args...)>::type;
  bool handle(Interface*); // catch exception
}; // mediator

template <typename Interface>
void mediator<Interface>::set_state(state st, const std::exception_ptr& exc)
{
  using namespace std;
  {
    unique_lock<mutex> lock(m_mut);
    m_st = st;
    m_tsk = 0;
    m_exc = exc;
  }
  m_cond.notify_one();
}

template <typename Interface>
void mediator<Interface>::exec(task* tsk)
{
  using namespace std;
  {
    unique_lock<mutex> lock(m_mut);
    m_cond.wait(lock, [&](){ return Idle == this->m_st || AfterFinish == this->m_st; });
    if (Idle == m_st)
    {
      m_st = Calling;
      m_tsk = tsk;
      m_exc = exception_ptr();
    }
  }
  m_cond.notify_one();

  unique_lock<mutex> lock(m_mut);
  m_cond.wait(lock, [&](){ return Idle == this->m_st || AfterFinish == this->m_st; });
  m_tsk = 0;
  if (!(m_exc == 0)) rethrow_exception(move(m_exc));
  if (AfterFinish == m_st) throw runtime_error("thread error");
}

template <typename Interface>
  template<typename Result, typename Fun, typename... Args>
Result mediator<Interface>::call(Fun&& f, Args&&... args)
{
  using namespace std;
  typedef function<Result(Interface*)> UnaryFun;
  UnaryFun uf(bind(forward<Fun>(f), forward<Args>(args)...));
  task_impl<Result, UnaryFun> tsk(uf);
  exec(&tsk);
  return tsk.result();
}

template <typename Interface>
bool mediator<Interface>::handle(Interface* arg)
{
  using namespace std;
  {
    unique_lock<mutex> lock(m_mut);
    m_cond.wait(lock, [&](){ return Calling == this->m_st || AfterFinish == this->m_st; });
    if (AfterFinish == m_st) return false;

    m_st = Idle;
    try  { m_tsk->exec(arg); }
    catch (const exception&)  { m_exc = current_exception(); }
  }
  m_cond.notify_one();
  return true;
} // mediator::

} } // brig::detail

#endif // BRIG_DETAIL_MEDIATOR_HPP
