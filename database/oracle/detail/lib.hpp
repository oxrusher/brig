// Andrew Naplavkov

#ifndef BRIG_DATABASE_ORACLE_DETAIL_LIB_HPP
#define BRIG_DATABASE_ORACLE_DETAIL_LIB_HPP

#include <brig/detail/dynamic_loading.hpp>
#include <oci.h>

namespace brig { namespace database { namespace oracle { namespace detail {

class lib {
  lib();

public:
  decltype(OCIAttrGet) *p_OCIAttrGet;
  decltype(OCIAttrSet) *p_OCIAttrSet;
  decltype(OCIBindByPos) *p_OCIBindByPos;
  decltype(OCIBindDynamic) *p_OCIBindDynamic;
  decltype(OCIBindObject) *p_OCIBindObject;
  decltype(OCIClientVersion) *p_OCIClientVersion;
  decltype(OCICollAppend) *p_OCICollAppend;
  decltype(OCICollGetElem) *p_OCICollGetElem;
  decltype(OCICollSize) *p_OCICollSize;
  decltype(OCIDefineByPos) *p_OCIDefineByPos;
  decltype(OCIDefineDynamic) *p_OCIDefineDynamic;
  decltype(OCIDefineObject) *p_OCIDefineObject;
  decltype(OCIDescriptorAlloc) *p_OCIDescriptorAlloc;
  decltype(OCIDescriptorFree) *p_OCIDescriptorFree;
  decltype(OCIEnvNlsCreate) *p_OCIEnvNlsCreate;
  decltype(OCIErrorGet) *p_OCIErrorGet;
  decltype(OCIHandleAlloc) *p_OCIHandleAlloc;
  decltype(OCIHandleFree) *p_OCIHandleFree;
  decltype(OCINumberFromInt) *p_OCINumberFromInt;
  decltype(OCINumberFromReal) *p_OCINumberFromReal;
  decltype(OCINumberToInt) *p_OCINumberToInt;
  decltype(OCINumberToReal) *p_OCINumberToReal;
  decltype(OCIObjectFree) *p_OCIObjectFree;
  decltype(OCIObjectNew) *p_OCIObjectNew;
  decltype(OCIParamGet) *p_OCIParamGet;
  decltype(OCIServerAttach) *p_OCIServerAttach;
  decltype(OCISessionBegin) *p_OCISessionBegin;
  decltype(OCIStmtExecute) *p_OCIStmtExecute;
  decltype(OCIStmtFetch2) *p_OCIStmtFetch2;
  decltype(OCIStmtPrepare) *p_OCIStmtPrepare;
  decltype(OCITransCommit) *p_OCITransCommit;
  decltype(OCITransRollback) *p_OCITransRollback;
  decltype(OCITypeByName) *p_OCITypeByName;

  bool empty() const  { return p_OCITypeByName == 0; }
  static lib& singleton()  { static lib s; return s; }
}; // lib

inline lib::lib() : p_OCITypeByName(0)
{
  auto handle = BRIG_DL_LIBRARY("oci.dll", "libclntsh.so");
  if (  handle
    && (p_OCIAttrGet = BRIG_DL_FUNCTION(handle, OCIAttrGet))
    && (p_OCIAttrSet = BRIG_DL_FUNCTION(handle, OCIAttrSet))
    && (p_OCIBindByPos = BRIG_DL_FUNCTION(handle, OCIBindByPos))
    && (p_OCIBindDynamic = BRIG_DL_FUNCTION(handle, OCIBindDynamic))
    && (p_OCIBindObject = BRIG_DL_FUNCTION(handle, OCIBindObject))
    && (p_OCIClientVersion = BRIG_DL_FUNCTION(handle, OCIClientVersion))
    && (p_OCICollAppend = BRIG_DL_FUNCTION(handle, OCICollAppend))
    && (p_OCICollGetElem = BRIG_DL_FUNCTION(handle, OCICollGetElem))
    && (p_OCICollSize = BRIG_DL_FUNCTION(handle, OCICollSize))
    && (p_OCIDefineByPos = BRIG_DL_FUNCTION(handle, OCIDefineByPos))
    && (p_OCIDefineDynamic = BRIG_DL_FUNCTION(handle, OCIDefineDynamic))
    && (p_OCIDefineObject = BRIG_DL_FUNCTION(handle, OCIDefineObject))
    && (p_OCIDescriptorAlloc = BRIG_DL_FUNCTION(handle, OCIDescriptorAlloc))
    && (p_OCIDescriptorFree = BRIG_DL_FUNCTION(handle, OCIDescriptorFree))
    && (p_OCIEnvNlsCreate = BRIG_DL_FUNCTION(handle, OCIEnvNlsCreate))
    && (p_OCIErrorGet = BRIG_DL_FUNCTION(handle, OCIErrorGet))
    && (p_OCIHandleAlloc = BRIG_DL_FUNCTION(handle, OCIHandleAlloc))
    && (p_OCIHandleFree = BRIG_DL_FUNCTION(handle, OCIHandleFree))
    && (p_OCINumberFromInt = BRIG_DL_FUNCTION(handle, OCINumberFromInt))
    && (p_OCINumberFromReal = BRIG_DL_FUNCTION(handle, OCINumberFromReal))
    && (p_OCINumberToInt = BRIG_DL_FUNCTION(handle, OCINumberToInt))
    && (p_OCINumberToReal = BRIG_DL_FUNCTION(handle, OCINumberToReal))
    && (p_OCIObjectFree = BRIG_DL_FUNCTION(handle, OCIObjectFree))
    && (p_OCIObjectNew = BRIG_DL_FUNCTION(handle, OCIObjectNew))
    && (p_OCIParamGet = BRIG_DL_FUNCTION(handle, OCIParamGet))
    && (p_OCIServerAttach = BRIG_DL_FUNCTION(handle, OCIServerAttach))
    && (p_OCISessionBegin = BRIG_DL_FUNCTION(handle, OCISessionBegin))
    && (p_OCIStmtExecute = BRIG_DL_FUNCTION(handle, OCIStmtExecute))
    && (p_OCIStmtFetch2 = BRIG_DL_FUNCTION(handle, OCIStmtFetch2))
    && (p_OCIStmtPrepare = BRIG_DL_FUNCTION(handle, OCIStmtPrepare))
    && (p_OCITransCommit = BRIG_DL_FUNCTION(handle, OCITransCommit))
    && (p_OCITransRollback = BRIG_DL_FUNCTION(handle, OCITransRollback))
     )  p_OCITypeByName = BRIG_DL_FUNCTION(handle, OCITypeByName);
} // lib::

} } } } // brig::database::oracle::detail

#endif // BRIG_DATABASE_ORACLE_DETAIL_LIB_HPP
