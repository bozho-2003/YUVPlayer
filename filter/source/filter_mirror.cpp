

#include "filter_mirror.h"



#define IMAGE_FILTER_NAME  "mirror"

#define MODULE_VERSION    "1.0"
#define MODULE_AUTHOR     "netjunegg@gmail.com"
#define MODULE_COMMENT    "图像左右翻转（镜像）"


const char *CFilter_Mirror::parameter_format =
"<filterinfo algname=\"mirror\" algtype=\"0\" onoff=\"1\" drawconfig =\"1\" drawresult =\"1\" vernum=\"0\">"
"  <!-- mirror -->"
"  <para>"
"    <label index = \"1\">"
"      <showname><![CDATA[base parameter]]></showname>"
"      <parainfo name=\"sensitivity\" type=\"list\">"
"        <!-- 灵敏度 -->"
"        <list name=\"high\" value = \"100\"/>"
"        <list name=\"medium\" value = \"80\"/>"
"        <list name=\"low\" value = \"60\"/>"
"        <val value = \"%d\"/>"
"        <modflag value = \"1\"/>"
"        <visibility value = \"1\"/>"
"        <showname><![CDATA[sensitivity ]]></showname>"
"        <comment><![CDATA[adjust sensitive]]></comment>"
"      </parainfo>"
"    </label>"
"    <label index = \"2\">"
"      <showname><![CDATA[advanced parameters]]></showname>"
"      <parainfo name=\"binarythresh\" type=\"int\">"
"        <min value = \"0\"/>"
"        <max value = \"255\"/>"
"        <val value = \"%d\"/>"
"        <modflag value = \"1\"/>"
"        <visibility value = \"0\"/>"
"        <showname><![CDATA[threshold]]></showname>"
"        <comment><![CDATA[0,255]]]></comment>"
"      </parainfo>"
"    </label>"
"  </para>"
"</filterinfo>";

CFilter_Mirror::CFilter_Mirror(){
	m_frame_size = 0;
	m_line_extra_bytes = 0;
	
	m_testVal = 1010;
	m_testVal2 = 1020;
	m_testStr = NULL;

	m_pBuf = NULL;
	m_bufSz = 0;
}

CFilter_Mirror::~CFilter_Mirror(){
	destroy();
}

// filter name
const char * CFilter_Mirror::getName(){
	return IMAGE_FILTER_NAME;
}

// other information
int  CFilter_Mirror::getModuleInfo(Module_Info *pInfo){
	sprintf(pInfo->author_name,MODULE_AUTHOR);
	sprintf(pInfo->comment,MODULE_COMMENT);
	sprintf(pInfo->version,MODULE_VERSION);
	return E_OK;
}

int  CFilter_Mirror::simpleConfig(int val, unsigned int val2, const char *str){
	m_testVal = val;
	m_testVal2 = val2;
	m_testStr = strdup(str);
	return 1;
}

int  CFilter_Mirror::selfConfig(char *infoBuf){

	return E_OK;
}

int  CFilter_Mirror::queryRuntimeInfo(char *infoBuf, int bufsz){
	if(infoBuf == NULL || bufsz <= 100)
		return -1;

    sprintf(infoBuf, CFilter_Mirror::parameter_format,
            m_frame_size, m_testVal);
            //m_frame_size, m_testVal, m_testVal2, m_testStr ? m_testStr:"NULL");

	return 1;
}

// 设置参数, format, width, height
int  CFilter_Mirror::setParams(RawImage_Info *pInfo){
	if(pInfo == NULL)
		return E_BAD_ARG;

	if(pInfo->raw_format != RAW_FORMAT_YUV444 
		&& pInfo->raw_format != RAW_FORMAT_BGR24 
		&& pInfo->raw_format != RAW_FORMAT_RGB24
		&& pInfo->raw_format != RAW_FORMAT_GREY8){
		return E_UNSUPPORTTED;
	}

	if(pInfo->width <= 0 || pInfo->height <= 0)
		return E_UNSUPPORTTED;

	m_inInfo = *pInfo;
	if(m_inInfo.raw_format == RAW_FORMAT_YUV444 ){
		m_frame_size = m_inInfo.width*m_inInfo.height;
		m_off1 = 0;
		m_off2 = m_frame_size;
		m_off3 = m_frame_size*2;
		m_frame_size *= 3;
		m_line_extra_bytes = 0;
	}
	else if(m_inInfo.raw_format == RAW_FORMAT_RGB24){
		m_frame_size = m_inInfo.width*m_inInfo.height;
		m_off1 = 0;
		m_off2 = 1;
		m_off3 = 2;
		m_frame_size *= 3;
		m_line_extra_bytes = 0;
	}
	else if(m_inInfo.raw_format == RAW_FORMAT_BGR24){
		m_line_extra_bytes = m_inInfo.width*3;
		while(m_line_extra_bytes&0x3) m_line_extra_bytes++;
		m_frame_size = m_line_extra_bytes*m_inInfo.height;
		m_line_extra_bytes -= m_inInfo.width*3;
		m_off1 = 2;
		m_off2 = 1;
		m_off3 = 0;
	}
	else if(m_inInfo.raw_format == RAW_FORMAT_GREY8){
		m_frame_size = m_inInfo.width*m_inInfo.height;
		m_off1 = 0;
		m_off2 = -1;
		m_off3 = -1;
		m_line_extra_bytes = 0;
	}
	else
		return E_UNSUPPORTTED;

	return E_OK;
}

int  CFilter_Mirror::update(void *pErr){
	if(m_frame_size > m_bufSz){
		if(m_pBuf)
			free(m_pBuf);
		m_pBuf = NULL;
		m_bufSz = 0;
	}
	
	if(m_pBuf == NULL){
		m_pBuf = (unsigned char*)malloc(m_frame_size);
		if(m_pBuf == NULL)
			return -2;
	}

	return 1;
}

int  CFilter_Mirror::getOutInfo(RawImage_Info *out){
	if(out == NULL)
		return E_BAD_ARG;
	*out = m_inInfo;
	return E_OK;
}


// filter
int  CFilter_Mirror::filter(unsigned char **ppData, int *pLen){
	if(ppData == NULL || pLen == NULL)
		return E_BAD_ARG;

	unsigned char *pData = *ppData;
	int nBytes = *pLen;
	if(ppData == NULL || nBytes < m_frame_size)
		return E_BAD_ARG;

	if(m_inInfo.raw_format == RAW_FORMAT_BGR24
		|| m_inInfo.raw_format == RAW_FORMAT_RGB24){
		unsigned char *p1, *p2;
		int i, j;
		
		p1 = pData;
		p2 = m_pBuf;
		p2 += (m_inInfo.width-1)*3;
		
		for(i = 0;i < m_inInfo.height;i++){
			for (j = 0;j < m_inInfo.width;j++){
				p2[0] = p1[0];
				p2[1] = p1[1];
				p2[2] = p1[2];
				p1 += 3;
				p2 -= 3;
			}
			p1 += m_line_extra_bytes;
			p2 -= m_line_extra_bytes;
		}
	}
	else{
		int i,j;
		unsigned char *p1,*p2,*p3;
		unsigned char *p21,*p22,*p23;

		p1 = pData + m_off1;
		p2 = pData + m_off2;
		p3 = pData + m_off3;
	
		p21 = m_pBuf + m_off1;
		p22 = m_pBuf + m_off2;
		p23 = m_pBuf + m_off3;
	
		for(i = 0;i < m_inInfo.height;i++){
			for(j = 0;j < m_inInfo.width;j++){
				if(m_off1 >= 0){
                    p21[m_inInfo.width - 1 - j] = *p1++;
				}
				if(m_off2 >= 0){
                    p22[m_inInfo.width - 1 - j] = *p2++;
				}
				if(m_off3 >= 0){
                    p23[m_inInfo.width - 1 - j] = *p3++;
				}
			}
            p21 += m_inInfo.width;
            p22 += m_inInfo.width;
            p23 += m_inInfo.width;
		}
	}

	*ppData = m_pBuf;
	return 1;
}

int  CFilter_Mirror::destroy(){
	if(m_pBuf)
		free(m_pBuf);
	m_pBuf = NULL;
	return 1;
}

CFilter_Mirror* CFilter_Mirror::getNewInstance()
{
    CFilter_Mirror *tmp = new CFilter_Mirror;
    if(tmp != NULL)
    {
        return tmp;
    }
    else
    {
        return NULL;
    }
}

int filter_mirror_get_interface(void **ppIntf){
	if(ppIntf == NULL)
		return -1;

	*ppIntf = new CFilter_Mirror;
	return *ppIntf != NULL;
}



