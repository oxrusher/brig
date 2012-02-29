// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_DETAIL_BINDING_DATETIME_HPP
#define BRIG_DATABASE_ORACLE_DETAIL_BINDING_DATETIME_HPP

#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <brig/database/oracle/detail/binding.hpp>
#include <brig/database/oracle/detail/handles.hpp>
#include <brig/database/oracle/detail/lib.hpp>
#include <stdexcept>

namespace brig { namespace database { namespace oracle { namespace detail {

class binding_datetime : public binding {
  OCIDateTime* m_dt;
  OCIInd m_ind;

  void alloc(handles* hnd)  { hnd->alloc_descriptor((void**)&m_dt, OCI_DTYPE_TIMESTAMP); }
  void bind(handles* hnd, size_t order, ub2 dty);
  void free()  { handles::free_descriptor((void**)&m_dt, OCI_DTYPE_TIMESTAMP); }

public:
  binding_datetime(handles* hnd, size_t order);
  binding_datetime(handles* hnd, size_t order, const ::boost::gregorian::date& r);
  binding_datetime(handles* hnd, size_t order, const ::boost::posix_time::ptime& r);
  virtual ~binding_datetime()  { free(); }
}; // binding_datetime

inline void binding_datetime::bind(handles* hnd, size_t order, ub2 dty)
{
  OCIBind* bnd(0);
  hnd->check(lib::singleton().p_OCIBindByPos(hnd->stmt, &bnd, hnd->err, ub4(order), &m_dt, sizeof(m_dt), dty, &m_ind, 0, 0, 0, 0, OCI_DEFAULT));
}

inline binding_datetime::binding_datetime(handles* hnd, size_t order) : m_dt(0), m_ind(OCI_IND_NULL)
{
  alloc(hnd);
  try  { bind(hnd, order, SQLT_TIMESTAMP); }
  catch (const std::exception&)  { free(); throw; }
}

inline binding_datetime::binding_datetime(handles* hnd, size_t order, const ::boost::gregorian::date& r) : m_dt(0), m_ind(sizeof(m_dt))
{
  alloc(hnd);
  try
  {
    hnd->check(lib::singleton().p_OCIDateTimeConstruct(hnd->env, hnd->err, m_dt
      , (sb2)r.year()
      , (ub1)r.month()
      , (ub1)r.day()
      , 0, 0, 0, 0, 0, 0
      ));
    bind(hnd, order, SQLT_DATE);
  }
  catch (const std::exception&)  { free(); throw; }
}

inline binding_datetime::binding_datetime(handles* hnd, size_t order, const ::boost::posix_time::ptime& r) : m_dt(0), m_ind(sizeof(m_dt))
{
  alloc(hnd);
  try
  {
    const ::boost::gregorian::date d(r.date());
    const ::boost::posix_time::time_duration t(r.time_of_day());
    hnd->check(lib::singleton().p_OCIDateTimeConstruct(hnd->env, hnd->err, m_dt
      , (sb2)d.year()
      , (ub1)d.month()
      , (ub1)d.day()
      , (ub1)t.hours()
      , (ub1)t.minutes()
      , (ub1)t.seconds()
      , 0, 0, 0
      ));
    bind(hnd, order, SQLT_TIMESTAMP);
  }
  catch (const std::exception&)  { free(); throw; }
} // binding_datetime::

} } } } // brig::database::oracle::detail

#endif // BRIG_DATABASE_ORACLE_DETAIL_BINDING_DATETIME_HPP
