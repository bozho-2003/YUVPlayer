

#ifndef FORMATER_I422_H_
#define FORMATER_I422_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "format.h"

class CFormater_I422:  public IYUV_Formater{
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
	CFormater_I422();
	~CFormater_I422();
	
	// format name
	const char * getName();
	// other information
	int  getModuleInfo(Module_Info *pInfo);
	
	int  simpleConfig(int val, unsigned int val2, const char *str);
	int  selfConfig();
	
	int  queryRuntimeInfo(char *infoBuf, int bufsz);
	
	// 设置源数据格式, width, height, field
	int  setParams(YUV_Params *params);
	int  update(void *pErr);
	
	// 查询输出数据格式, format, width, height
	int  getOutInfo(RawImage_Info *out);
	
	int  getMinBlockSize();
	int  getMinBlockFrameNum();
	
	// 获取buffer, buf大小应为minBlockSize
	int  getBuffer(unsigned char **ppBuf);
	int  putData(unsigned char *pBuf);
	
	int  getFrameIndex();
	int  setFrameIndex(int index);
	
	// offset: < 0: 上一帧, ==0: 当前帧, >0: 下一帧
	int  getFrame(int offset, unsigned char *pBuf, int bufsz);  // a
	
	int  toYourFormat(RawImage_Info *src_info, const unsigned char *src_buf, int src_bufsz, unsigned char *dst_buf, int *p_dst_bufsz);
	
	int  destroy();
};


int formater_i422_get_interface(void **ppIntf);


#endif


