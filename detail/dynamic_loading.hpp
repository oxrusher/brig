// Andrew Naplavkov

#ifndef BRIG_DETAIL_DYNAMIC_LOADING_HPP
#define BRIG_DETAIL_DYNAMIC_LOADING_HPP

#ifdef _WIN32
  #include <windows.h>
  #define BRIG_DL_LIBRARY(win, lin) LoadLibraryA(win)
  #define BRIG_DL_FUNCTION(lib, func) (decltype(func)*)GetProcAddress(lib, BRIG_STRINGIFY(func))

#elif defined __linux__
  #include <dlfcn.h>
  #define BRIG_DL_LIBRARY(win, lin) dlopen(lin, RTLD_LAZY)
  #define BRIG_DL_FUNCTION(lib, func) (decltype(func)*)dlsym(lib, BRIG_STRINGIFY(func))

#endif

#define BRIG_STRINGIFY(str) #str

#endif // BRIG_DETAIL_DYNAMIC_LOADING_HPP
