#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {Y8, YUV420, YUV422, YUV444, UYVY, RGB888, RGB32} FrameType;

extern int myYUVToRGB(unsigned char *pYuv,unsigned char *pRgb,
                int width,int height,int yuvType,int RGBType);

extern int myRGBToYUV(unsigned char *pRgb,unsigned char *pYuv,
               int width,int height,int rgbType,int yuvType);

extern int myGetIntFromStr(const char *str, int *pVal);

extern int myGetRangeUint(const char *s,char c,int *pmin,int *pmax,int sort);

#endif // COMMON_H
