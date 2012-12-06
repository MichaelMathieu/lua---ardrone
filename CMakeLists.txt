CMAKE_MINIMUM_REQUIRED(VERSION 2.6 FATAL_ERROR)
CMAKE_POLICY(VERSION 2.6)
FIND_PACKAGE(Torch REQUIRED)

LINK_DIRECTORIES(/usr/local/lib)

SET(src THpp.cpp ardrone.cpp ffmpeg_wrapper.c)
SET(luasrc init.lua)

ADD_TORCH_PACKAGE(ardrone "${src}" "${luasrc}" "ARdrone")
TARGET_LINK_LIBRARIES(ardrone luaT TH pthread avformat avcodec)
