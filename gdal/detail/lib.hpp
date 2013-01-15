// Andrew Naplavkov

#ifndef BRIG_GDAL_DETAIL_LIB_HPP
#define BRIG_GDAL_DETAIL_LIB_HPP

#include <brig/detail/dynamic_loading.hpp>
#include <stdexcept>

#if defined(_WIN32) && !defined(CPL_DISABLE_STDCALL)
  #define CPL_STDCALL __stdcall
  #include <brig/detail/decoration.hpp>
  #define BRIG_GDAL_DL_FUNCTION(lib, fun) (decltype(fun)*)GetProcAddress(lib, BRIG_DECORATION(fun))

#else
  #define BRIG_GDAL_DL_FUNCTION(lib, fun) BRIG_DL_FUNCTION(lib, fun)

#endif

#include <ogr_api.h>
#include <ogr_srs_api.h>

namespace brig { namespace gdal { namespace detail {

class lib {
  lib();

public:
  decltype(GDALVersionInfo) *p_GDALVersionInfo;
  decltype(OGR_Dr_CreateDataSource) *p_OGR_Dr_CreateDataSource;
  decltype(OGR_DS_CreateLayer) *p_OGR_DS_CreateLayer;
  decltype(OGR_DS_DeleteLayer) *p_OGR_DS_DeleteLayer;
  decltype(OGR_DS_Destroy) *p_OGR_DS_Destroy;
  decltype(OGR_DS_GetLayer) *p_OGR_DS_GetLayer;
  decltype(OGR_DS_GetLayerByName) *p_OGR_DS_GetLayerByName;
  decltype(OGR_DS_GetLayerCount) *p_OGR_DS_GetLayerCount;
  decltype(OGR_DS_SyncToDisk) *p_OGR_DS_SyncToDisk;
  decltype(OGR_F_Create) *p_OGR_F_Create;
  decltype(OGR_F_Destroy) *p_OGR_F_Destroy;
  decltype(OGR_F_GetFieldAsBinary) *p_OGR_F_GetFieldAsBinary;
  decltype(OGR_F_GetFieldAsDouble) *p_OGR_F_GetFieldAsDouble;
  decltype(OGR_F_GetFieldAsInteger) *p_OGR_F_GetFieldAsInteger;
  decltype(OGR_F_GetFieldAsString) *p_OGR_F_GetFieldAsString;
  decltype(OGR_F_GetFieldDefnRef) *p_OGR_F_GetFieldDefnRef;
  decltype(OGR_F_GetGeometryRef) *p_OGR_F_GetGeometryRef;
  decltype(OGR_F_SetFieldBinary) *p_OGR_F_SetFieldBinary;
  decltype(OGR_F_SetFieldDouble) *p_OGR_F_SetFieldDouble;
  decltype(OGR_F_SetFieldInteger) *p_OGR_F_SetFieldInteger;
  decltype(OGR_F_SetFieldString) *p_OGR_F_SetFieldString;
  decltype(OGR_F_SetGeometryDirectly) *p_OGR_F_SetGeometryDirectly;
  decltype(OGR_F_UnsetField) *p_OGR_F_UnsetField;
  decltype(OGR_FD_GetFieldCount) *p_OGR_FD_GetFieldCount;
  decltype(OGR_FD_GetFieldDefn) *p_OGR_FD_GetFieldDefn;
  decltype(OGR_FD_GetFieldIndex) *p_OGR_FD_GetFieldIndex;
  decltype(OGR_Fld_Create) *p_OGR_Fld_Create;
  decltype(OGR_Fld_Destroy) *p_OGR_Fld_Destroy;
  decltype(OGR_Fld_GetNameRef) *p_OGR_Fld_GetNameRef;
  decltype(OGR_Fld_GetType) *p_OGR_Fld_GetType;
  decltype(OGR_G_CreateFromWkb) *p_OGR_G_CreateFromWkb;
  decltype(OGR_G_ExportToWkb) *p_OGR_G_ExportToWkb;
  decltype(OGR_G_WkbSize) *p_OGR_G_WkbSize;
  decltype(OGR_L_CreateFeature) *p_OGR_L_CreateFeature;
  decltype(OGR_L_CreateField) *p_OGR_L_CreateField;
  decltype(OGR_L_GetExtent) *p_OGR_L_GetExtent;
  decltype(OGR_L_GetLayerDefn) *p_OGR_L_GetLayerDefn;
  decltype(OGR_L_GetName) *p_OGR_L_GetName;
  decltype(OGR_L_GetNextFeature) *p_OGR_L_GetNextFeature;
  decltype(OGR_L_GetSpatialRef) *p_OGR_L_GetSpatialRef;
  decltype(OGR_L_ResetReading) *p_OGR_L_ResetReading;
  decltype(OGR_L_SetAttributeFilter) *p_OGR_L_SetAttributeFilter;
  decltype(OGR_L_SetSpatialFilter) *p_OGR_L_SetSpatialFilter;
  decltype(OGR_L_SetSpatialFilterRect) *p_OGR_L_SetSpatialFilterRect;
  decltype(OGR_L_SyncToDisk) *p_OGR_L_SyncToDisk;
  decltype(OGRFree) *p_OGRFree;
  decltype(OGRGetDriverByName) *p_OGRGetDriverByName;
  decltype(OGROpen) *p_OGROpen;
  decltype(OGRRegisterAll) *p_OGRRegisterAll;
  decltype(OSRAutoIdentifyEPSG) *p_OSRAutoIdentifyEPSG;
  decltype(OSRDestroySpatialReference) *p_OSRDestroySpatialReference;
  decltype(OSRExportToProj4) *p_OSRExportToProj4;
  decltype(OSRGetAuthorityCode) *p_OSRGetAuthorityCode;
  decltype(OSRGetAuthorityName) *p_OSRGetAuthorityName;
  decltype(OSRImportFromProj4) *p_OSRImportFromProj4;
  decltype(OSRNewSpatialReference) *p_OSRNewSpatialReference;

  bool empty() const  { return p_OSRNewSpatialReference == 0; }
  static lib& singleton()  { static lib s; return s; }
  static std::string ogr_geom_wkb()  { return "ogr_geom_wkb"; }
  static void check(OGRErr r)  { if (OGRERR_NONE != r) throw std::runtime_error("GDAL error"); }
}; // lib

inline lib::lib() : p_OSRNewSpatialReference(0)
{
  auto handle = BRIG_DL_LIBRARY("gdal19.dll", ""); // todo:
  if (  handle
    && (p_GDALVersionInfo = BRIG_GDAL_DL_FUNCTION(handle, GDALVersionInfo))
    && (p_OGR_Dr_CreateDataSource = BRIG_GDAL_DL_FUNCTION(handle, OGR_Dr_CreateDataSource))
    && (p_OGR_DS_CreateLayer = BRIG_GDAL_DL_FUNCTION(handle, OGR_DS_CreateLayer))
    && (p_OGR_DS_DeleteLayer = BRIG_GDAL_DL_FUNCTION(handle, OGR_DS_DeleteLayer))
    && (p_OGR_DS_Destroy = BRIG_GDAL_DL_FUNCTION(handle, OGR_DS_Destroy))
    && (p_OGR_DS_GetLayer = BRIG_GDAL_DL_FUNCTION(handle, OGR_DS_GetLayer))
    && (p_OGR_DS_GetLayerByName = BRIG_GDAL_DL_FUNCTION(handle, OGR_DS_GetLayerByName))
    && (p_OGR_DS_GetLayerCount = BRIG_GDAL_DL_FUNCTION(handle, OGR_DS_GetLayerCount))
    && (p_OGR_DS_SyncToDisk = BRIG_GDAL_DL_FUNCTION(handle, OGR_DS_SyncToDisk))
    && (p_OGR_F_Create = BRIG_GDAL_DL_FUNCTION(handle, OGR_F_Create))
    && (p_OGR_F_Destroy = BRIG_GDAL_DL_FUNCTION(handle, OGR_F_Destroy))
    && (p_OGR_F_GetFieldAsBinary = BRIG_GDAL_DL_FUNCTION(handle, OGR_F_GetFieldAsBinary))
    && (p_OGR_F_GetFieldAsDouble = BRIG_GDAL_DL_FUNCTION(handle, OGR_F_GetFieldAsDouble))
    && (p_OGR_F_GetFieldAsInteger = BRIG_GDAL_DL_FUNCTION(handle, OGR_F_GetFieldAsInteger))
    && (p_OGR_F_GetFieldAsString = BRIG_GDAL_DL_FUNCTION(handle, OGR_F_GetFieldAsString))
    && (p_OGR_F_GetFieldDefnRef = BRIG_GDAL_DL_FUNCTION(handle, OGR_F_GetFieldDefnRef))
    && (p_OGR_F_GetGeometryRef = BRIG_GDAL_DL_FUNCTION(handle, OGR_F_GetGeometryRef))
    && (p_OGR_F_SetFieldBinary = BRIG_GDAL_DL_FUNCTION(handle, OGR_F_SetFieldBinary))
    && (p_OGR_F_SetFieldDouble = BRIG_GDAL_DL_FUNCTION(handle, OGR_F_SetFieldDouble))
    && (p_OGR_F_SetFieldInteger = BRIG_GDAL_DL_FUNCTION(handle, OGR_F_SetFieldInteger))
    && (p_OGR_F_SetFieldString = BRIG_GDAL_DL_FUNCTION(handle, OGR_F_SetFieldString))
    && (p_OGR_F_SetGeometryDirectly = BRIG_GDAL_DL_FUNCTION(handle, OGR_F_SetGeometryDirectly))
    && (p_OGR_F_UnsetField = BRIG_GDAL_DL_FUNCTION(handle, OGR_F_UnsetField))
    && (p_OGR_FD_GetFieldCount = BRIG_GDAL_DL_FUNCTION(handle, OGR_FD_GetFieldCount))
    && (p_OGR_FD_GetFieldDefn = BRIG_GDAL_DL_FUNCTION(handle, OGR_FD_GetFieldDefn))
    && (p_OGR_FD_GetFieldIndex = BRIG_GDAL_DL_FUNCTION(handle, OGR_FD_GetFieldIndex))
    && (p_OGR_Fld_Create = BRIG_GDAL_DL_FUNCTION(handle, OGR_Fld_Create))
    && (p_OGR_Fld_Destroy = BRIG_GDAL_DL_FUNCTION(handle, OGR_Fld_Destroy))
    && (p_OGR_Fld_GetNameRef = BRIG_GDAL_DL_FUNCTION(handle, OGR_Fld_GetNameRef))
    && (p_OGR_Fld_GetType = BRIG_GDAL_DL_FUNCTION(handle, OGR_Fld_GetType))
    && (p_OGR_G_CreateFromWkb = BRIG_GDAL_DL_FUNCTION(handle, OGR_G_CreateFromWkb))
    && (p_OGR_G_ExportToWkb = BRIG_GDAL_DL_FUNCTION(handle, OGR_G_ExportToWkb))
    && (p_OGR_G_WkbSize = BRIG_GDAL_DL_FUNCTION(handle, OGR_G_WkbSize))
    && (p_OGR_L_CreateFeature = BRIG_GDAL_DL_FUNCTION(handle, OGR_L_CreateFeature))
    && (p_OGR_L_CreateField = BRIG_GDAL_DL_FUNCTION(handle, OGR_L_CreateField))
    && (p_OGR_L_GetExtent = BRIG_GDAL_DL_FUNCTION(handle, OGR_L_GetExtent))
    && (p_OGR_L_GetLayerDefn = BRIG_GDAL_DL_FUNCTION(handle, OGR_L_GetLayerDefn))
    && (p_OGR_L_GetName = BRIG_GDAL_DL_FUNCTION(handle, OGR_L_GetName))
    && (p_OGR_L_GetNextFeature = BRIG_GDAL_DL_FUNCTION(handle, OGR_L_GetNextFeature))
    && (p_OGR_L_GetSpatialRef = BRIG_GDAL_DL_FUNCTION(handle, OGR_L_GetSpatialRef))
    && (p_OGR_L_ResetReading = BRIG_GDAL_DL_FUNCTION(handle, OGR_L_ResetReading))
    && (p_OGR_L_SetAttributeFilter = BRIG_GDAL_DL_FUNCTION(handle, OGR_L_SetAttributeFilter))
    && (p_OGR_L_SetSpatialFilter = BRIG_GDAL_DL_FUNCTION(handle, OGR_L_SetSpatialFilter))
    && (p_OGR_L_SetSpatialFilterRect = BRIG_GDAL_DL_FUNCTION(handle, OGR_L_SetSpatialFilterRect))
    && (p_OGR_L_SyncToDisk = BRIG_GDAL_DL_FUNCTION(handle, OGR_L_SyncToDisk))
    && (p_OGRFree = BRIG_GDAL_DL_FUNCTION(handle, OGRFree))
    && (p_OGRGetDriverByName = BRIG_GDAL_DL_FUNCTION(handle, OGRGetDriverByName))
    && (p_OGROpen = BRIG_GDAL_DL_FUNCTION(handle, OGROpen))
    && (p_OGRRegisterAll = BRIG_GDAL_DL_FUNCTION(handle, OGRRegisterAll))
    && (p_OSRAutoIdentifyEPSG = BRIG_GDAL_DL_FUNCTION(handle, OSRAutoIdentifyEPSG))
    && (p_OSRDestroySpatialReference = BRIG_GDAL_DL_FUNCTION(handle, OSRDestroySpatialReference))
    && (p_OSRExportToProj4 = BRIG_GDAL_DL_FUNCTION(handle, OSRExportToProj4))
    && (p_OSRGetAuthorityCode = BRIG_GDAL_DL_FUNCTION(handle, OSRGetAuthorityCode))
    && (p_OSRGetAuthorityName = BRIG_GDAL_DL_FUNCTION(handle, OSRGetAuthorityName))
    && (p_OSRImportFromProj4 = BRIG_GDAL_DL_FUNCTION(handle, OSRImportFromProj4))
     )  p_OSRNewSpatialReference = BRIG_GDAL_DL_FUNCTION(handle, OSRNewSpatialReference);

  if (!empty()) p_OGRRegisterAll();
} // lib::

} } } // brig::gdal::detail

#endif // BRIG_GDAL_DETAIL_LIB_HPP
