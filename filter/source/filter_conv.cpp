#include <QImage>
#include "uti_xmlparse.h"
#include "filter_conv.h"

#define IMAGE_FILTER_NAME  "ConvFilter"

#define MODULE_VERSION    "0.0.1"
#define MODULE_AUTHOR     "bozho.2003@gmail.com"
#define MODULE_COMMENT    "A simple conv filter for image process, which don't change image format and resolution"

const char *CFilter_ConvFilter::parameter_format =
    "<filterinfo algname=\"conv\" algtype=\"0\" onoff=\"1\" drawconfig =\"1\" drawresult =\"1\" vernum=\"0\">"
    "  <!-- conv filter -->"
    "  <para>"
    "    <label index = \"1\">"
    "      <showname><![CDATA[base parameters]]></showname>"
    "      <parainfo name=\"k-w\" type=\"int\">"
    "        <min value = \"0\"/>"
    "        <max value = \"15\"/>"
    "        <val value = \"%d\"/>"
    "        <modflag value = \"1\"/>"
    "        <visibility value = \"1\"/>"
    "        <showname><![CDATA[kernel-width]]></showname>"
    "        <comment><![CDATA[kennel width 0-15]]]></comment>"
    "      </parainfo>"
    "      <parainfo name=\"k-h\" type=\"int\">"
    "        <min value = \"0\"/>"
    "        <max value = \"15\"/>"
    "        <val value = \"%d\"/>"
    "        <modflag value = \"1\"/>"
    "        <visibility value = \"1\"/>"
    "        <showname><![CDATA[kernel-height]]></showname>"
    "        <comment><![CDATA[kernel hwight 0-15]]]></comment>"
    "      </parainfo>"
    "      <parainfo name=\"kernel\" type=\"str\">"
    "        <min value = \"0.0\"/>"
    "        <max value = \"255.0\"/>"
    "        <val value = \"%s\"/>"
    "        <modflag value = \"1\"/>"
    "        <visibility value = \"0\"/>"
    "        <showname><![CDATA[y gain]]></showname>"
    "        <comment><![CDATA[0,255]]]></comment>"
    "      </parainfo>"
    "    </label>"
    "  </para>"
    "</filterinfo>";


CFilter_ConvFilter::CFilter_ConvFilter(){
	m_frame_size = 0;
    m_line_extra_bytes = 0;
}

CFilter_ConvFilter::~CFilter_ConvFilter(){
	destroy();
}

// filter name
const char * CFilter_ConvFilter::getName(){
	return IMAGE_FILTER_NAME;
}

// other information
int  CFilter_ConvFilter::getModuleInfo(Module_Info *pInfo){
	sprintf(pInfo->author_name,MODULE_AUTHOR);
	sprintf(pInfo->comment,MODULE_COMMENT);
	sprintf(pInfo->version,MODULE_VERSION);
	return E_OK;
}

int  CFilter_ConvFilter::simpleConfig(int val, unsigned int val2, const char *str){
	return 1;
}

int  CFilter_ConvFilter::selfConfig(char *infoBuf)
{
    TXMLHandlePtr ptHan, ptHanLoad;
    TXMLElePtr t1, t2, t3, t4, t5, t6,t7;

    int idx;
    int i = 0;

    ptHan = XMLParseOpenFromStr(infoBuf);
    t1 = XMLGetRootElement(ptHan, "filterinfo");
    t2 = XMLGetFirstChildElement(t1, "para");

    for(TXMLElePtr tLabel = XMLGetFirstChildElement(t2, "label");
        tLabel != NULL;
        tLabel = XMLGetNextSiblingElement(tLabel))
    {
        const char *ele_name = XMLGetEleName(tLabel);
        if(strcmp(ele_name, "label") == 0)
        {
            t1 = XMLGetFirstChildElement(tLabel, "showname");

            XMLGetElementAttributel32(tLabel, "index", &idx);

            for(TXMLElePtr tParaInfo = XMLGetFirstChildElement(tLabel, "parainfo");
                tParaInfo != NULL;
                tParaInfo = XMLGetNextSiblingElement(tParaInfo))
            {
                const char *ele_name = XMLGetEleName(tParaInfo);
                if(strcmp(ele_name, "parainfo") == 0)
                {
                    const char *type = XMLGetElementAttributes8(tParaInfo, "type");
                    const char *name = XMLGetElementAttributes8(tParaInfo, "name");
                    if (type != NULL && name != NULL)
                    {
                        if (strcmp(type, "float") == 0)
                        {
                            TXMLElePtr tMin = XMLGetFirstChildElement(tParaInfo, "min");
                            TXMLElePtr tMax = XMLGetFirstChildElement(tParaInfo, "max");
                            TXMLElePtr tVal = XMLGetFirstChildElement(tParaInfo, "val");
                            TXMLElePtr tVisibility = XMLGetFirstChildElement(tParaInfo, "visibility");
                            TXMLElePtr tName = XMLGetFirstChildElement(tParaInfo, "showname");
                            float min, max, val, visibility_flag;
                            XMLGetElementAttributef32(tMin, "value", &min);
                            XMLGetElementAttributef32(tMax, "value", &max);
                            XMLGetElementAttributef32(tVal, "value", &val);
                            XMLGetElementAttributef32(tVisibility, "value", &visibility_flag);
                            const char *name_str = XMLGetElementText(tName);
                            if(strcmp(name, "y_gain") == 0)
                            {
                                m_gain1 = val;
                            }
                            else if(strcmp(name, "u_gain") == 0)
                            {
                                m_gain2 = val;
                            }
                            else if(strcmp(name, "v_gain") == 0)
                            {
                                m_gain3 = val;
                            }
                        }
                        else if (strcmp(type, "int") == 0)
                        {
                            TXMLElePtr tMin = XMLGetFirstChildElement(tParaInfo, "min");
                            TXMLElePtr tMax = XMLGetFirstChildElement(tParaInfo, "max");
                            TXMLElePtr tVal = XMLGetFirstChildElement(tParaInfo, "val");
                            TXMLElePtr tVisibility = XMLGetFirstChildElement(tParaInfo, "visibility");
                            TXMLElePtr tName = XMLGetFirstChildElement(tParaInfo, "showname");
                            int min, max, val, visibility_flag;
                            XMLGetElementAttributel32(tMin, "value", &min);
                            XMLGetElementAttributel32(tMax, "value", &max);
                            XMLGetElementAttributel32(tVal, "value", &val);
                            XMLGetElementAttributel32(tVisibility, "value", &visibility_flag);
                            const char *name_str = XMLGetElementText(tName);
                            if(strcmp(name, "type") == 0)
                            {
                                m_type = val;
                            }
                        }
                    }
                }
            }
        }
    }

	return E_OK;
}

int  CFilter_ConvFilter::queryRuntimeInfo(char *infoBuf, int bufsz){
	if(infoBuf == NULL || bufsz <= 100)
		return -1;

    sprintf(infoBuf, CFilter_ConvFilter::parameter_format, m_type,
            m_gain1, m_gain2, m_gain3);

	return 1;
}

int  CFilter_ConvFilter::setParams(RawImage_Info *pInfo)
{
	if(pInfo == NULL)
		return E_BAD_ARG;

	if(pInfo->raw_format != RAW_FORMAT_YUV444 
       && pInfo->raw_format != RAW_FORMAT_YUV420
       && pInfo->raw_format != RAW_FORMAT_BGR24
       && pInfo->raw_format != RAW_FORMAT_RGB24
       && pInfo->raw_format != RAW_FORMAT_GREY8){
		return E_UNSUPPORTTED;
	}

	if(pInfo->width <= 0 || pInfo->height <= 0)
		return E_UNSUPPORTTED;

	m_inInfo = *pInfo;
    if(m_inInfo.raw_format == RAW_FORMAT_YUV420 )
    {
        m_frame_size = m_inInfo.width*m_inInfo.height;
        m_off1 = 0;
        m_off2 = m_frame_size;
        m_off3 = m_frame_size * 5 / 4;
        m_frame_size = m_inInfo.width*m_inInfo.height * 3 / 2;
        m_line_extra_bytes = 0;
    }
    else if(m_inInfo.raw_format == RAW_FORMAT_YUV444 ){
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

int  CFilter_ConvFilter::update(void *pErr){
	return 1;
}

int  CFilter_ConvFilter::getOutInfo(RawImage_Info *out){
	if(out == NULL)
		return E_BAD_ARG;
	*out = m_inInfo;
	return E_OK;
}

int  CFilter_ConvFilter::filter_444_type0(unsigned char **ppData, int *pLen)
{
    if(ppData == NULL || pLen == NULL)
        return E_BAD_ARG;

    unsigned char *pData = *ppData;
    int nBytes = *pLen;
    if(ppData == NULL || nBytes < m_frame_size)
        return E_BAD_ARG;

    int i,j;
    unsigned char *p1,*p2,*p3;
    p1 = pData + m_off1;
    p2 = pData + m_off2;
    p3 = pData + m_off3;

    for(i = 0;i < m_inInfo.height;i++){
        for(j = 0;j < m_inInfo.width;j++)
        {
            unsigned long tmp;
            if(m_off1 >= 0){
                tmp = *p1 + m_gain1;
                if(tmp > 255) tmp = 255;
                *p1++ = tmp;
            }
            if(m_off2 >= 0){
                tmp = *p2 + m_gain2;
                if(tmp > 255) tmp = 255;
                *p2++ = tmp;
            }
            if(m_off3 >= 0){
                tmp = *p3 + m_gain3;
                if(tmp > 255) tmp = 255;
                *p3++ = tmp;
            }
        }
    }
    return 1;
}
int  CFilter_ConvFilter::filter_yuv420_type0(unsigned char **ppData, int *pLen)
{
    if(ppData == NULL || pLen == NULL)
        return E_BAD_ARG;

    unsigned char *pData = *ppData;
    int nBytes = *pLen;
    if(ppData == NULL || nBytes < m_frame_size)
        return E_BAD_ARG;

    int i,j;
    unsigned char *p1,*p2,*p3;
    p1 = pData + m_off1;
    p2 = pData + m_off2;
    p3 = pData + m_off3;

    for(i = 0;i < m_inInfo.height;i++)
    {
        for(j = 0;j < m_inInfo.width;j++)
        {
            unsigned long tmp;
            if(m_off1 >= 0){
                tmp = *p1 + m_gain1;
                if(tmp > 255) tmp = 255;
                *p1++ = tmp;
            }
        }
    }
    for(i = 0;i < (m_inInfo.height >> 1);i++)
    {
        for(j = 0;j < (m_inInfo.width >> 1);j++)
        {
            unsigned long tmp;
            if(m_off2 >= 0){
                tmp = *p2 + m_gain2;
                if(tmp > 255) tmp = 255;
                *p2++ = tmp;
            }
            if(m_off3 >= 0){
                tmp = *p3 + m_gain3;
                if(tmp > 255) tmp = 255;
                *p3++ = tmp;
            }
        }
    }
    return 1;
}

int  CFilter_ConvFilter::filter_444_type1(unsigned char **ppData, int *pLen)
{
    if(ppData == NULL || pLen == NULL)
        return E_BAD_ARG;

    unsigned char *pData = *ppData;
    int nBytes = *pLen;
    if(ppData == NULL || nBytes < m_frame_size)
        return E_BAD_ARG;

    int i,j;
    unsigned char *p1,*p2,*p3;
    p1 = pData + m_off1;
    p2 = pData + m_off2;
    p3 = pData + m_off3;


    for(i = 0;i < m_inInfo.height;i++){
        for(j = 0;j < m_inInfo.width;j++)
        {
            unsigned long tmp;
            if(m_off1 >= 0){
                tmp = *p1 * m_gain1;
                if(tmp > 255) tmp = 255;
                *p1++ = tmp;
            }
            if(m_off2 >= 0){
                tmp = *p2 * m_gain2;
                if(tmp > 255) tmp = 255;
                *p2++ = tmp;
            }
            if(m_off3 >= 0){
                tmp = *p3 * m_gain3;
                if(tmp > 255) tmp = 255;
                *p3++ = tmp;
            }
        }
    }

    return 1;
}

int  CFilter_ConvFilter::filter_yuv420_type1(unsigned char **ppData, int *pLen)
{
    if(ppData == NULL || pLen == NULL)
        return E_BAD_ARG;

    unsigned char *pData = *ppData;
    int nBytes = *pLen;
    if(ppData == NULL || nBytes < m_frame_size)
        return E_BAD_ARG;

    int i,j;
    unsigned char *p1,*p2,*p3;
    p1 = pData + m_off1;
    p2 = pData + m_off2;
    p3 = pData + m_off3;

    for(i = 0;i < m_inInfo.height;i++){
        for(j = 0;j < m_inInfo.width;j++)
        {
            unsigned long tmp;
            if(m_off1 >= 0){
                tmp = *p1 * m_gain1;
                if(tmp > 255) tmp = 255;
                *p1++ = tmp;
            }
        }
    }
    for(i = 0;i < (m_inInfo.height / 2);i++){
        for(j = 0;j < (m_inInfo.width / 2);j++)
        {
            unsigned long tmp;
            if(m_off2 >= 0){
                tmp = *p2 * m_gain2;
                if(tmp > 255) tmp = 255;
                *p2++ = tmp;
            }
            if(m_off3 >= 0){
                tmp = *p3 * m_gain3;
                if(tmp > 255) tmp = 255;
                *p3++ = tmp;
            }
        }
    }

    return 1;
}

// filter
int  CFilter_ConvFilter::filter(unsigned char **ppData, int *pLen){

    if(m_type)
    {
        if(m_inInfo.raw_format == RAW_FORMAT_YUV420 )
        {
            return filter_yuv420_type0(ppData, pLen);
        }
        else if((m_inInfo.raw_format == RAW_FORMAT_YUV444) ||
                (m_inInfo.raw_format == RAW_FORMAT_RGB24) ||
                (m_inInfo.raw_format == RAW_FORMAT_BGR24) ||
                (m_inInfo.raw_format == RAW_FORMAT_GREY8))
        {
            return filter_444_type0(ppData, pLen);
        }
    }
    else
    {
        if(m_inInfo.raw_format == RAW_FORMAT_YUV420 )
        {
            return filter_yuv420_type1(ppData, pLen);
        }
        else if((m_inInfo.raw_format == RAW_FORMAT_YUV444) ||
                (m_inInfo.raw_format == RAW_FORMAT_RGB24) ||
                (m_inInfo.raw_format == RAW_FORMAT_BGR24) ||
                (m_inInfo.raw_format == RAW_FORMAT_GREY8))
        {
            return filter_444_type1(ppData, pLen);
        }
    }


    return 1;
}


int  CFilter_ConvFilter::destroy(){
	return 1;
}

CFilter_ConvFilter* CFilter_ConvFilter::getNewInstance()
{
    CFilter_ConvFilter *tmp = new CFilter_ConvFilter;
    if(tmp != NULL)
    {
        return tmp;
    }
    else
    {
        return NULL;
    }
}

int filter_conv_get_interface(void **ppIntf){
	if(ppIntf == NULL)
		return -1;

    *ppIntf = new CFilter_ConvFilter;
	return *ppIntf != NULL;
}



