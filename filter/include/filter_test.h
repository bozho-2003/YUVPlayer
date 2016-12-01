

#ifndef FILTER_TEST_H_
#define FILTER_TEST_H_
#include "format.h"


class CFilter_Test: public IImage_Filter{
public:
	RawImage_Info m_inInfo;
	int m_frame_size;
	int m_line_extra_bytes;
	int m_off1;
	int m_off2;
	int m_off3;
	
    int m_type;
	float m_gain1;
	float m_gain2;
	float m_gain3;
	
	int m_testVal;
	unsigned int m_testVal2;
	char *m_testStr;

    static char *parameter_format;

	CFilter_Test();
	~CFilter_Test();

	// filter name
    const char * getName();
	// other information
    int  getModuleInfo(Module_Info *pInfo);
	
    int  simpleConfig(int val, unsigned int val2, const char *str);
    int  selfConfig(char *infoBuf);
	
    int  queryRuntimeInfo(char *infoBuf, int bufsz);

    // format, width, height
    int  setParams(RawImage_Info *pInfo);
    int  update(void *pErr);

    int  getOutInfo(RawImage_Info *out);

	// filter
    int  filter(unsigned char **ppData, int *pLen);
	
    int  destroy();
    CFilter_Test* getNewInstance();
};



int filter_test_get_interface(void **ppIntf);



#endif



