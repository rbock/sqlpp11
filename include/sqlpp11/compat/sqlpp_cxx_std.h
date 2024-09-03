#pragma once

#ifdef _MSVC_LANG
#define SQLPP_CXX_STD _MSVC_LANG
#else
#define SQLPP_CXX_STD __cplusplus
#endif
