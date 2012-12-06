extern "C" {
#include <TH.h>
#include <luaT.h>
}

#include "THpp.hpp"
using namespace std;
using namespace TH;

#include "ardrone.h"
#include "ffmpeg_wrapper.h"
#include<cstring>

static int InitArdrone(lua_State* L) {
  setLuaState(L);
  string pipe = FromLuaStack<std::string>(1);

  Ardrone* ar = new Ardrone();

  ar->pipeC = fopen(pipe.c_str(), "w");
  
  // TODO : To improve custom size and stream
  // Init ffmpeg_wrapper
  //init_video();

  lua_pushlightuserdata(L, (void*)ar);
  return 1;
}

static int TakeoffArdrone(lua_State* L) {
  Ardrone* ar = (Ardrone*)lua_touserdata(L, 1);
  int takeoff = FromLuaStack<int>(2);
  char buffer[8];
  sprintf(buffer, "$T%d#", (int)(takeoff != 0));
  fwrite(buffer,  sizeof(char), strlen(buffer), ar->pipeC);
  fflush(ar->pipeC);
  return 0;
}

static int EmergencyArdrone(lua_State* L) {
  setLuaState(L);
  Ardrone* ar = (Ardrone*)lua_touserdata(L, 1);
  int takeoff = FromLuaStack<int>(2);
  char buffer[8];
  sprintf(buffer, "$E%d#", (int)(takeoff != 0));
  fwrite(buffer,  sizeof(char), strlen(buffer), ar->pipeC);
  fflush(ar->pipeC);
  return 0;
}

static int CommandArdrone(lua_State* L) {
  setLuaState(L);
  Ardrone* ar = (Ardrone*)lua_touserdata(L, 1);
  float fb = FromLuaStack<float>(2);
  float lr = FromLuaStack<float>(3);
  float rot= FromLuaStack<float>(4);
  float alt= FromLuaStack<float>(5);
  char buffer[64];
  sprintf(buffer, "$C%f,%f,%f,%f#",fb, lr, rot, alt);
  fwrite(buffer, sizeof(char), strlen(buffer), ar->pipeC);
  fflush(ar->pipeC);
  return 0;
}

static int InitVideoArdrone(lua_State* L) {
  setLuaState(L);
  init_video();
  return 0;
}

static int GetFrameArdrone(lua_State* L) {
  setLuaState(L);
  Tensor<float> frame = FromLuaStack<Tensor<float> >(1);
  get_frame(frame.data(), 640, 360);
  return 0;
}

static int ReleaseArdrone(lua_State* L) {
  setLuaState(L);
  Ardrone* ar = (Ardrone*)lua_touserdata(L, 1);
  fclose(ar->pipeC);
  delete ar;
  return 0;
}

//============================================================
// Register functions in LUA
//


#define torch_(NAME)        TH_CONCAT_3(torch_, Real, NAME)
#define torch_Tensor        TH_CONCAT_STRING_3(torch.,Real,Tensor)
#define libardrone_(NAME)   TH_CONCAT_3(libardrone_, Real, NAME)

static const luaL_reg libardrone_init [] = {
  {"initArdrone", InitArdrone},
  {"commandArdrone", CommandArdrone},
  {"takeoffArdrone", TakeoffArdrone},
  {"emergencyArdrone", EmergencyArdrone},
  {"getFrameArdrone", GetFrameArdrone},
  {"initVideoArdrone", InitVideoArdrone},
  {"releaseArdrone", ReleaseArdrone},
  {NULL, NULL}
};

#include "generic/ardrone.cpp"
#include "THGenerateFloatTypes.h"

LUA_EXTERNC DLL_EXPORT int luaopen_libardrone(lua_State *L) {
  luaL_register(L, "libardrone", libardrone_init);
  
  libardrone_FloatMain_init(L);
  libardrone_DoubleMain_init(L);
  
  return 1;
}
