// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_DETAIL_HANDLES_HPP
#define BRIG_DATABASE_ORACLE_DETAIL_HANDLES_HPP

#include <brig/database/oracle/detail/lib.hpp>
#include <brig/unicode/transform.hpp>
#include <cstdint>
#include <stdexcept>
#include <string>

namespace brig { namespace database { namespace oracle { namespace detail {

struct handles
{
  OCIEnv* env;
  OCIError* err;
  OCIServer* srv;
  OCISvcCtx* svc;
  OCISession* ses;
  OCIType* geom;
  OCIStmt* stmt;

  handles() : env(0), err(0), srv(0), svc(0), ses(0), geom(0), stmt(0)  {}

  void check(sword r);

  void alloc_handle(void** p_handle, ub4 type);
  void alloc_descriptor(void** p_desc, ub4 type);
  static void free_handle(void** p_handle, ub4 type);
  static void free_descriptor(void** p_desc, ub4 type);

  uint32_t get_int(const OCINumber* num, OCIInd ind = OCI_IND_NOTNULL);
  void set_int(uint32_t val, OCINumber* num, OCIInd* ind = 0);
  double get_real(const OCINumber* num, OCIInd ind = OCI_IND_NOTNULL);
  void set_real(double val, OCINumber* num, OCIInd* ind = 0);
  uint32_t get_count(const OCIArray* arr, OCIInd ind);
  OCINumber* get_number(const OCIArray* arr, uint32_t offset);
}; // handles

inline void handles::check(sword r)
{
  using namespace std;

  if (OCI_SUCCESS == r || OCI_SUCCESS_WITH_INFO == r) return;
  basic_string<utext> msg;
  if (0 != err && OCI_ERROR == r)
  {
    ub4 record(1);
    sb4 code(0);
    utext buf[512];
    while (OCI_SUCCESS == lib::singleton().p_OCIErrorGet(err, record++, 0, &code, (text*)buf, sizeof(buf), OCI_HTYPE_ERROR))
    {
      if (!msg.empty()) msg += utext(32);
      msg += buf;
    }
  }
  throw runtime_error(msg.empty()? "OCI error": brig::unicode::transform<char>(msg));
}

inline void handles::alloc_handle(void** p_handle, ub4 type)
{
  void* handle(0);
  check(lib::singleton().p_OCIHandleAlloc(env, &handle, type, 0, 0));
  *p_handle = handle;
}

inline void handles::alloc_descriptor(void** p_desc, ub4 type)
{
  void* desc(0);
  check(lib::singleton().p_OCIDescriptorAlloc(env, &desc, type, 0, 0));
  *p_desc = desc;
}

inline void handles::free_handle(void** p_handle, ub4 type)
{
  if (0 == *p_handle) return;
  void* handle(*p_handle); *p_handle = 0;
  lib::singleton().p_OCIHandleFree(handle, type);
}

inline void handles::free_descriptor(void** p_desc, ub4 type)
{
  if (*p_desc == 0) return;
  void* desc(*p_desc); *p_desc = 0;
  lib::singleton().p_OCIDescriptorFree(desc, type);
}

inline uint32_t handles::get_int(const OCINumber* num, OCIInd ind)
{
  if (0 == num || OCI_IND_NULL == ind) throw std::runtime_error("OCI error");
  uint32_t val(0);
  check(lib::singleton().p_OCINumberToInt(err, num, sizeof(val), OCI_NUMBER_UNSIGNED, &val));
  return val;
}

inline void handles::set_int(uint32_t val, OCINumber* num, OCIInd* ind)
{
  if (0 == num) throw std::runtime_error("OCI error");
  check(lib::singleton().p_OCINumberFromInt(err, &val, sizeof(val), OCI_NUMBER_UNSIGNED, num));
  if (ind) *ind = OCI_IND_NOTNULL;
}

inline double handles::get_real(const OCINumber* num, OCIInd ind)
{
  if (0 == num || OCI_IND_NULL == ind) throw std::runtime_error("OCI error");
  double val(0);
  check(lib::singleton().p_OCINumberToReal(err, num, sizeof(val), &val));
  return val;
}

inline void handles::set_real(double val, OCINumber* num, OCIInd* ind)
{
  if (0 == num) throw std::runtime_error("OCI error");
  check(lib::singleton().p_OCINumberFromReal(err, &val, sizeof(val), num));
  if (ind) *ind = OCI_IND_NOTNULL;
}

inline uint32_t handles::get_count(const OCIArray* arr, OCIInd ind)
{
  if (0 == arr) throw std::runtime_error("OCI error");
  sb4 val(0);
  if (OCI_IND_NULL != ind) check(lib::singleton().p_OCICollSize(env, err, arr, &val));
  if (val < 0) throw std::runtime_error("OCI error");
  return val;
}

inline OCINumber* handles::get_number(const OCIArray* arr, uint32_t offset)
{
  if (0 == arr) throw std::runtime_error("OCI error");
  boolean exists(false);
  OCINumber* num(0);
  check(lib::singleton().p_OCICollGetElem(env, err, arr, offset, &exists, (void**)&num, 0));
  if (!exists) throw std::runtime_error("OCI error");
  return num;
} // handles::

} } } } // brig::database::oracle::detail

#endif // BRIG_DATABASE_ORACLE_DETAIL_HANDLES_HPP
