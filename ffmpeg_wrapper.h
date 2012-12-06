#ifndef __FFMPEG_WRAPPER_H__
#define __FFMPEG_WRAPPER_H__


int init_video();
int get_frame(float * data, int width, int height);

#endif
