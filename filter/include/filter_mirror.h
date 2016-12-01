

#ifndef FILTER_MIRROR_H_
#define FILTER_MIRROR_H_
#include "format.h"


class CFilter_Mirror: public IImage_Filter{
public:
	RawImage_Info m_inInfo;
	int m_frame_size;
	int m_line_extra_bytes;
	int m_off1;
	int m_off2;
	int m_off3;
	
	int m_testVal;
	unsigned int m_testVal2;
	char *m_testStr;
	
	unsigned char *m_pBuf;
	int m_bufSz;

    static char *parameter_format;

	CFilter_Mirror();
	~CFilter_Mirror();

	// filter name
    const char * getName();
	// other information
    int  getModuleInfo(Module_Info *pInfo);
	
    int  simpleConfig(int val, unsigned int val2, const char *str);
    int  selfConfig(char *infoBuf);
	
    int  queryRuntimeInfo(char *infoBuf, int bufsz);

	// 设置参数, format, width, height
    int  setParams(RawImage_Info *pInfo);
    int  update(void *pErr);

    int  getOutInfo(RawImage_Info *out);

	// filter
    int  filter(unsigned char **ppData, int *pLen);
	
    int  destroy();
    CFilter_Mirror* getNewInstance();
};



int filter_mirror_get_interface(void **ppIntf);



#endif



