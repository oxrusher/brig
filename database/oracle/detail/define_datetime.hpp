// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_DETAIL_DEFINE_DATETIME_HPP
#define BRIG_DATABASE_ORACLE_DETAIL_DEFINE_DATETIME_HPP

#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <brig/database/oracle/detail/define.hpp>
#include <brig/database/oracle/detail/handles.hpp>
#include <brig/database/oracle/detail/lib.hpp>
#include <brig/database/variant.hpp>
#include <exception>

namespace brig { namespace database { namespace oracle { namespace detail {

class define_datetime : public define {
  handles* m_hnd;
  const bool m_has_time;
  OCIDateTime* m_dt;
  OCIInd m_ind;
  void free()  { handles::free_descriptor((void**)&m_dt, OCI_DTYPE_TIMESTAMP); }
public:
  define_datetime(handles* hnd, size_t order, bool has_time);
  virtual ~define_datetime()  { free(); }
  virtual void operator()(variant& var);
}; // define_datetime

inline define_datetime::define_datetime(handles* hnd, size_t order, bool has_time) : m_hnd(hnd), m_has_time(has_time), m_dt(0), m_ind(sizeof(m_dt))
{
  m_hnd->alloc_descriptor((void**)&m_dt, OCI_DTYPE_TIMESTAMP);
  try
  {
    OCIDefine* def(0);
    m_hnd->check(lib::singleton().p_OCIDefineByPos(m_hnd->stmt, &def, m_hnd->err, ub4(order), &m_dt, sizeof(m_dt), m_has_time? SQLT_TIMESTAMP: SQLT_DATE, &m_ind, 0, 0, OCI_DEFAULT));
  }
  catch (const std::exception&)  { free(); throw; }
}

inline void define_datetime::operator()(variant& var)
{
  if (OCI_IND_NULL == m_ind)
    var = null_t();
  else
  {
    sb2 year(0); ub1 month(0); ub1 day(0);
    m_hnd->check(lib::singleton().p_OCIDateTimeGetDate(m_hnd->env, m_hnd->err, m_dt, &year, &month, &day));
    if (m_has_time)
    {
      ub1 hour(0); ub1 minute(0); ub1 sec(0); ub4 fsec(0);
      m_hnd->check(lib::singleton().p_OCIDateTimeGetTime(m_hnd->env, m_hnd->err, m_dt, &hour, &minute, &sec, &fsec));
      var = boost::posix_time::ptime
        ( boost::gregorian::date(year, month, day)
        , boost::posix_time::time_duration(hour, minute, sec)
        );
    }
    else
      var = boost::gregorian::date(year, month, day);
  }
} // define_datetime::

} } } } // brig::database::oracle::detail

#endif // BRIG_DATABASE_ORACLE_DETAIL_DEFINE_DATETIME_HPP
