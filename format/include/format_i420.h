#ifndef FORMAT_I420_H
#define FORMAT_I420_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "format.h"


class CFormater_I420:  public IYUV_Formater{
    unsigned char *m_buf;
    int m_bufsz;

    int m_block_size;

    int m_frame_num;
    int m_index;

    unsigned short m_width;
    unsigned short m_height;
    int m_field;
    int m_y_size;

    int m_id;

    void _init();

public:
    CFormater_I420();
    ~CFormater_I420();

    // format name
    const char * getName();
    // other information
    int  getModuleInfo(Module_Info *pInfo);

    int  simpleConfig(int val, unsigned int val2, const char *str);
    int  selfConfig();

    int  queryRuntimeInfo(char *infoBuf, int bufsz);

    int  setParams(YUV_Params *params);
    int  update(void *pErr);

    int  getOutInfo(RawImage_Info *out);

    int  getMinBlockSize();
    int  getMinBlockFrameNum();

    int  getBuffer(unsigned char **ppBuf);
    int  putData(unsigned char *pBuf);

    int  getFrameIndex();
    int  setFrameIndex(int index);

    // offset: < 0: previous frame , ==0: current frame, >0: next frame
    int  getFrame(int offset, unsigned char *pBuf, int bufsz);  // a

    // format convert:
    // For a single convert, this method will be called twich probably.
    // 1. if dat_buf is null, this method will return dst buffer size to let caller malloc memory.
    // 2. if dat_buf is not null, format convert will be executed.
    // 3. if the format is not supported, return minus one.
    // 4. This method will be dependent furthest.

    int  toYourFormat(RawImage_Info *src_info, const unsigned char *src_buf, int src_bufsz, unsigned char *dst_buf, int *p_dst_bufsz);

    int  destroy();
};


int formater_i420_get_interface(void **ppIntf);

#endif // FORMAT_I420_H
