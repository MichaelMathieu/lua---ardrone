
#include <stdio.h>
#include <stdlib.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavformat/avio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

#define PACKET_BUFFERED_NB 4

AVFormatContext* pFormatCtx = NULL;
AVCodecContext *pCodecCtx = NULL;
AVCodec *pCodec = NULL;
AVFrame* pFrameYUV[PACKET_BUFFERED_NB];
pthread_mutex_t lock[PACKET_BUFFERED_NB];
int freeSlot[PACKET_BUFFERED_NB];
unsigned int wArrayIdx = 0;
unsigned int rArrayIdx = 0;
int videoStreamIndex = -1;
pthread_t tId;

int srcX = 640;
int srcY = 360;
int dstX = 640; 
int dstY = 360;
int cnt = 0;

void* taskBufferFrame(void *arg){
  int frame_decoded = 0;
  int fail = 0;
  pthread_mutex_t * tmpLock;
  AVPacket tmpPacket;
  av_init_packet(&tmpPacket);

  while(1){
    tmpLock = &lock[wArrayIdx];
    pthread_mutex_lock(tmpLock);
    if(av_read_frame(pFormatCtx,&tmpPacket) >= 0 && 
       tmpPacket.stream_index == videoStreamIndex){
      avcodec_decode_video2(pCodecCtx, pFrameYUV[wArrayIdx], &frame_decoded, &tmpPacket);
      av_free_packet(&tmpPacket);
      av_init_packet(&tmpPacket);
      if(frame_decoded){
	freeSlot[wArrayIdx] = 0;
	cnt++;
	wArrayIdx = (wArrayIdx + 1) % PACKET_BUFFERED_NB;
      }
      else{
	printf("buffer can't decode %u \n", wArrayIdx);
	fail = 1;
      }
    }
    else{
      printf("buffer can't read %u \n", wArrayIdx);
      av_free_packet(&tmpPacket);
      av_init_packet(&tmpPacket);
      fail = 1;
    }
    pthread_mutex_unlock(tmpLock);
    if(fail){
      usleep(5000);
      fail = 0;
    }
  }
}


int decode_frame(float * output, AVFrame* src, int width, int height){
  int i, j;
  for(i = 0 ; i < height; i++){
    for(j = 0 ; j < width; j++){
      output[i*width + j] = *(src->data[0]+i * src->linesize[0] + j) / 255.0;
    }
  }
  return 1;
}


int get_frame(float * data, int width, int height){
  int wait_frame = 1;
  pthread_mutex_t * tmpLock;

  while(wait_frame){
    rArrayIdx = wArrayIdx;
    if(rArrayIdx == 0)
      rArrayIdx = PACKET_BUFFERED_NB - 1;
    else
      rArrayIdx = rArrayIdx - 1;
    tmpLock = &lock[rArrayIdx];
    //printf("reader wants lock %u \n", rArrayIdx);
    pthread_mutex_lock(tmpLock);
    //printf("reader gets lock %u \n", rArrayIdx);
    if(freeSlot[rArrayIdx] == 1){
      pthread_mutex_unlock(tmpLock);
      //printf("reader has nothing to read %u \n", rArrayIdx);
      usleep(5000);
    }
    else{
      decode_frame(data, pFrameYUV[rArrayIdx], width, height);
      freeSlot[rArrayIdx] = 1;
      pthread_mutex_unlock(tmpLock);
      return 1;
    }
  }
  return 0;
}



int init_video(){
  int i;

  av_register_all();
  avcodec_register_all();
  avformat_network_init();


  printf("\topenning stream %s ... ", "http://192.168.1.1:5555");
  if(avformat_open_input(&pFormatCtx, /*"video_test.mp4"*/"http://192.168.1.1:5555",NULL,NULL) != 0){
    fprintf(stderr, "could not open stream\n");
    return EXIT_FAILURE;
  }
  if(pFormatCtx == NULL){
    fprintf(stderr, "could not open stream\n");
    return EXIT_FAILURE;
  }
  printf("done\n");

  av_dump_format(pFormatCtx, 0, "video_test.mp4", 0);

  //search video stream
  printf("\tsearching video stream among %d streams... \n", pFormatCtx->nb_streams);
  for(i = 0;i<pFormatCtx->nb_streams;i++){
    if(pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
      videoStreamIndex = i;
  }
  printf("done\n");

  pCodecCtx=pFormatCtx->streams[videoStreamIndex]->codec;
  pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
  if(pCodec==NULL) {
    fprintf(stderr, "Unsupported codec!\n");
    return EXIT_FAILURE; // Codec not found
  }

  // Open codec
  printf("\topening codec ... ");
  if(avcodec_open(pCodecCtx, pCodec)<0)
    return EXIT_FAILURE; // Could not open codec
  printf("done\n");

  printf("\tDetected size : width %d, height %d \n", pCodecCtx->width, pCodecCtx->height);

  printf("\tInitializing decoded frame ... ");
  for(i = 0; i < PACKET_BUFFERED_NB; i++){
    pFrameYUV[i]=avcodec_alloc_frame();
    if(pFrameYUV[i]==NULL) {
      fprintf(stderr, "Frame not initialized!\n");
      return EXIT_FAILURE; 
    }
  }
  printf("done\n");

  printf("\tInitializing Mutexes ... \n");
  for(i = 0; i < PACKET_BUFFERED_NB; i++){
    if (pthread_mutex_init(&lock[i], NULL) != 0){
      fprintf(stderr, "mutex init failed\n");
      return EXIT_FAILURE;
    }
  }
  printf("done\n");  

  printf("\tInitializing buffer status flags ... \n");
  for(i = 0; i < PACKET_BUFFERED_NB; i++){
    freeSlot[i] = 1;
  }
  printf("done\n");  

  printf("\tLaunching task ... \n");
  if(pthread_create(&(tId), NULL, &taskBufferFrame, NULL) != 0){
    fprintf(stderr, "Can't create task\n");
    return EXIT_FAILURE;
  }
  printf("done\n");
  
  printf("\tPlaying stream ... ");
  av_read_play(pFormatCtx);
  printf("done\n");
  
  printf("Init done\n");
  
  return 0;
}

/* int main(int argc, char** argv) { */
  
/*   int frameNb = 200; */
/*   //float data[640*360]; */
/*   float *data = NULL; */
/*   int recorded_frame = 0; */
  
/*   data = (float*)malloc(sizeof(float) * 640*360*frameNb); */
/*   if(data == NULL){ */
/*     printf("Can't malloc...\n"); */
/*     return -1; */
/*   } */
/*   printf("Initialization...\n"); */
/*   init(); */
/*   sleep(1); */
/*   while(recorded_frame < frameNb){ */
/*     get_frame((data+640*360*recorded_frame), 640, 360, 640, 1); */
/*     Mat_<float> display(360, 640, data); */
/*     namedWindow("display"); */
/*     imshow("display", display); */
/*     cvWaitKey(1); */
/*   } */
/*   return (0); */
/* } */

 

