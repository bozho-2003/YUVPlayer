#ifndef FORMAT_H
#define FORMAT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// use it
#define E_OK              1
#define E_BAD_ARG         -2   // parameter wrong
#define E_UNSUPPORTTED    -2   // parameter which is not supported.
#define E_NO_MEMORY       -3   // malloc memeory failed.
#define E_NO_INIT         -4   // uninitiated
#define E_OOPS            -5   // state wrong
#define E_USER            -6


#define MODULE_NAME_MAX_LENGTH      64
#define MODULE_VERSION_MAX_LENGTH   64
#define MODULE_AUTHOR_MAX_LENGTH    64
#define MODULE_COMMENT_MAX_LENGTH   1024

// Module information
typedef struct Module_Info{
    char version[MODULE_VERSION_MAX_LENGTH];		//
    char author_name[MODULE_AUTHOR_MAX_LENGTH];	//
    char comment[MODULE_COMMENT_MAX_LENGTH];		//
}Module_Info;


#define FIELD_UNKNOWN         0
#define FIELD_ALL             1     // 
#define FIELD_TOP_ONLY        2     // 
#define FIELD_BOTTOM_ONLY     3     // 
#define FIELD_TOP_BOTTOM_SEQ  4     // show field, frist top field, then bottom field
#define FIELD_TOP_BOTTOM_SP   5     // top field up


typedef struct YUV_Params{
    unsigned short width;
    unsigned short height;
    int field;
}YUV_Params;


#define RAW_FORMAT_UNKNOWN    0
#define RAW_FORMAT_YUV444     1
#define RAW_FORMAT_RGB24      2   // rgb,rgb...
#define RAW_FORMAT_BGR24      3   // bmp image
#define RAW_FORMAT_GREY8      4
#define RAW_FORMAT_USER       10  // user defined format


typedef struct RawImage_Info{
    int raw_format;
    int width;
    int height;
    int frame_size;
}RawImage_Info;


class IYUV_Formater{
public:
    // format name
    virtual const char * getName() = 0;
    // other information
    virtual int  getModuleInfo(Module_Info *pInfo) = 0;

    // two config method
    virtual int  simpleConfig(int val, unsigned int val2, const char *str) = 0;
    virtual int  selfConfig() = 0;

    // For debuging, inner state can be got through this methond.
    virtual int  queryRuntimeInfo(char *infoBuf, int bufsz) = 0;

    // set source format, width, height, field, return -1 if format is not supported.
    virtual int  setParams(YUV_Params *params) = 0;
    // update inner state, this method will be called after setParams.
    virtual int  update(void *pErr = NULL) = 0;

    // query output data format
    virtual int  getOutInfo(RawImage_Info *out) = 0;

    virtual int  getMinBlockSize() = 0;
    virtual int  getMinBlockFrameNum() = 0;

    // its size is minBlockSize.
    virtual int  getBuffer(unsigned char **ppBuf) = 0;
    // update frame data
    virtual int  putData(unsigned char *pBuf) = 0;

    virtual int  getFrameIndex() = 0;
    virtual int  setFrameIndex(int index) = 0;

    // offset: < 0: previous frame , ==0: current frame, >0: next frame
    virtual int  getFrame(int offset, unsigned char *pBuf, int bufsz) = 0;  // a

    // format convert:
    // For a single convert, this method will be called twich probably.
    // 1. if dat_buf is null, this method will return dst buffer size to let caller malloc memory.
    // 2. if dat_buf is not null, format convert will be executed.
    // 3. if the format is not supported, return minus one.
    // 4. This method will be dependent furthest.
    virtual int  toYourFormat(RawImage_Info *src_info, const unsigned char *src_buf, int src_bufsz, unsigned char *dst_buf, int *p_dst_bufsz) = 0;

    //release memory.
    virtual int  destroy() = 0;
};



class IImage_Filter{
public:
    // filter name
    virtual const char * getName() = 0;
    // other information
    virtual int  getModuleInfo(Module_Info *pInfo) = 0;

    virtual int  simpleConfig(int val, unsigned int val2, const char *str) = 0;
    virtual int  selfConfig(char *infoBuf) = 0;

    virtual int  queryRuntimeInfo(char *infoBuf, int bufsz) = 0;

    virtual int  setParams(RawImage_Info *pInfo) = 0;
    virtual int  update(void *pErr = NULL) = 0;

    virtual int  getOutInfo(RawImage_Info *out) = 0;

    // filter
    virtual int  filter(unsigned char **ppData, int *pLen) = 0;
    virtual int  destroy() = 0;

    virtual IImage_Filter* getNewInstance() = 0;
};


#endif // FORMAT_H
