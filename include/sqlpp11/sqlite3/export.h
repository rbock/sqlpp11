#pragma once

#ifdef _WIN32
  #ifdef SQLPP11_SHARED
    #ifdef SQLPP11_COMPILING_DLL
      #define SQLPP11_SQLITE3_EXPORT __declspec(dllexport)
    #else
      #define SQLPP11_SQLITE3_EXPORT __declspec(dllimport)
    #endif
  #else
    #define SQLPP11_SQLITE3_EXPORT
  #endif
#else
  #define SQLPP11_SQLITE3_EXPORT
#endif
