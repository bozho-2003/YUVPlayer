

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
    unsigned char *m_img_out;
	
    //int m_type;
    int m_kernel_w;
    int m_kernel_h;
    char m_kernel_str[1024];
    int m_kernel_size;
    int m_kernel_val[16 * 16];


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
    int  conv_filter_yuv420(unsigned char **ppData, int *pLen);
    int  conv_filter_444(unsigned char **ppData, int *pLen);

    int  conv_filter_pixel(unsigned char *p1, unsigned char *p1_dst, int row, int column);

    int  filter(unsigned char **ppData, int *pLen);
	
    int  destroy();
    CFilter_ConvFilter* getNewInstance();
};



int filter_conv_get_interface(void **ppIntf);



#endif  //__CONF_FILTER__
