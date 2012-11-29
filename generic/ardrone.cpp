#ifndef TH_GENERIC_FILE
#define TH_GENERIC_FILE "generic/ardrone.cpp"
#else

#include "THpp.hpp"
#include<iostream>
#include "ardrone.h"

//#define VERBOSE_DEBUG

using namespace TH;
using namespace std;

//============================================================
// Register functions in LUA
//

static const luaL_reg libardrone_(Main__) [] = {
  {NULL, NULL}  /* sentinel */
};

LUA_EXTERNC DLL_EXPORT int libardrone_(Main_init) (lua_State *L) {
  luaT_pushmetatable(L, torch_Tensor);
  luaT_registeratname(L, libardrone_(Main__), "libardrone");
  lua_pop(L,1);
  return 1;
}

#endif
