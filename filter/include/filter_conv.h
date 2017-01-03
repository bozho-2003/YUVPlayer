

#ifndef __CONF_FILTER__
#define __CONF_FILTER__
#include "format.h"


class CFilter_ConvFilter: public IImage_Filter{
public:
	RawImage_Info m_inInfo;
	int m_frame_size;
	int m_line_extra_bytes;
	int m_off1;
	int m_off2;
	int m_off3;
	
    //int m_type;
    char kernel_str[1024];
    int kernel_val[16 * 16];

    int m_testVal;
    unsigned int m_testVal2;
    char *m_testStr;

    static const char *parameter_format;

    CFilter_ConvFilter();
    ~CFilter_ConvFilter();

    const char * getName();   	// filter name
    int  getModuleInfo(Module_Info *pInfo);  // other information
	
    int  simpleConfig(int val, unsigned int val2, const char *str);
    int  selfConfig(char *infoBuf);
	
    int  queryRuntimeInfo(char *infoBuf, int bufsz);

    // format, width, height
    int  setParams(RawImage_Info *pInfo);
    int  update(void *pErr);

    int  getOutInfo(RawImage_Info *out);

    // filter
    int  filter_444_type0(unsigned char **ppData, int *pLen);
    int  filter_yuv420_type0(unsigned char **ppData, int *pLen);
    int  filter_444_type1(unsigned char **ppData, int *pLen);
    int  filter_yuv420_type1(unsigned char **ppData, int *pLen);

    int  filter(unsigned char **ppData, int *pLen);
	
    int  destroy();
    CFilter_ConvFilter* getNewInstance();
};



int filter_conv_get_interface(void **ppIntf);



#endif  //__CONF_FILTER__
