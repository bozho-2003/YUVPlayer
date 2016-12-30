#include <QImage>
#include "uti_xmlparse.h"
#include "filter_test.h"




#define IMAGE_FILTER_NAME  "SimpleAdjust"

#define MODULE_VERSION    "0.0.1"
#define MODULE_AUTHOR     "netjunegg@gmail.com"
#define MODULE_COMMENT    "一个简单滤镜, 调节分量增益\r\n" \
						"不改变格式, 不改变分辨率"

char *CFilter_Test::parameter_format =
"<filterinfo algname=\"mirror\" algtype=\"0\" onoff=\"1\" drawconfig =\"1\" drawresult =\"1\" vernum=\"0\">"
"  <!-- mirror -->"
"  <para>"
"    <label index = \"1\">"
"      <showname><![CDATA[base parameters]]></showname>"
"      <parainfo name=\"type\" type=\"int\">"
"        <min value = \"0\"/>"
"        <max value = \"1\"/>"
"        <val value = \"%d\"/>"
"        <modflag value = \"1\"/>"
"        <visibility value = \"0\"/>"
"        <showname><![CDATA[type]]></showname>"
"        <comment><![CDATA[0,255]]]></comment>"
"      </parainfo>"
"      <parainfo name=\"y_gain\" type=\"float\">"
"        <min value = \"0.0\"/>"
"        <max value = \"255.0\"/>"
"        <val value = \"%f\"/>"
"        <modflag value = \"1\"/>"
"        <visibility value = \"0\"/>"
"        <showname><![CDATA[y gain]]></showname>"
"        <comment><![CDATA[0,255]]]></comment>"
"      </parainfo>"
"      <parainfo name=\"u_gain\" type=\"float\">"
"        <min value = \"0.0\"/>"
"        <max value = \"255.0\"/>"
"        <val value = \"%f\"/>"
"        <modflag value = \"1\"/>"
"        <visibility value = \"0\"/>"
"        <showname><![CDATA[u gain]]></showname>"
"        <comment><![CDATA[0,255]]]></comment>"
"      </parainfo>"
"      <parainfo name=\"v_gain\" type=\"float\">"
"        <min value = \"0.0\"/>"
"        <max value = \"255.0\"/>"
"        <val value = \"%f\"/>"
"        <modflag value = \"1\"/>"
"        <visibility value = \"0\"/>"
"        <showname><![CDATA[v gain]]></showname>"
"        <comment><![CDATA[0,255]]]></comment>"
"      </parainfo>"
"    </label>"
"  </para>"
"</filterinfo>";


CFilter_Test::CFilter_Test(){
	m_frame_size = 0;
	m_line_extra_bytes = 0;

    m_type = 0;
    m_gain1 = 1.0;
    m_gain2 = 1.0;
    m_gain3 = 1.0;
	
	m_testVal = 1010;
	m_testVal2 = 1020;
	m_testStr = NULL;
}

CFilter_Test::~CFilter_Test(){
	destroy();
}

// filter name
const char * CFilter_Test::getName(){
	return IMAGE_FILTER_NAME;
}

// other information
int  CFilter_Test::getModuleInfo(Module_Info *pInfo){
	sprintf(pInfo->author_name,MODULE_AUTHOR);
	sprintf(pInfo->comment,MODULE_COMMENT);
	sprintf(pInfo->version,MODULE_VERSION);
	return E_OK;
}

int  CFilter_Test::simpleConfig(int val, unsigned int val2, const char *str){
	m_testVal = val;
	m_testVal2 = val2;
	m_testStr = strdup(str);
	return 1;
}

int  CFilter_Test::selfConfig(char *infoBuf)
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

int  CFilter_Test::queryRuntimeInfo(char *infoBuf, int bufsz){
	if(infoBuf == NULL || bufsz <= 100)
		return -1;

    sprintf(infoBuf, CFilter_Test::parameter_format, m_type,
            m_gain1, m_gain2, m_gain3);

	return 1;
}

// 设置参数, format, width, height
int  CFilter_Test::setParams(RawImage_Info *pInfo){
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

int  CFilter_Test::update(void *pErr){
	return 1;
}

int  CFilter_Test::getOutInfo(RawImage_Info *out){
	if(out == NULL)
		return E_BAD_ARG;
	*out = m_inInfo;
	return E_OK;
}


// filter
int  CFilter_Test::filter(unsigned char **ppData, int *pLen){
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
    if(m_type)
    {
        if(m_inInfo.raw_format == RAW_FORMAT_YUV420 )
        {
            for(i = 0;i < m_inInfo.height;i++){
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
            for(i = 0;i < (m_inInfo.height >> 1);i++){
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
        }
        else if((m_inInfo.raw_format == RAW_FORMAT_YUV444) ||
                (m_inInfo.raw_format == RAW_FORMAT_RGB24) ||
                (m_inInfo.raw_format == RAW_FORMAT_BGR24) ||
                (m_inInfo.raw_format == RAW_FORMAT_GREY8))
        {
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
        }
    }
    else
    {
        if(m_inInfo.raw_format == RAW_FORMAT_YUV420 )
        {
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
        }
        else if((m_inInfo.raw_format == RAW_FORMAT_YUV444) ||
                (m_inInfo.raw_format == RAW_FORMAT_RGB24) ||
                (m_inInfo.raw_format == RAW_FORMAT_BGR24) ||
                (m_inInfo.raw_format == RAW_FORMAT_GREY8))
        {
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
        }
    }


	return 1;
}

int  CFilter_Test::destroy(){
	return 1;
}

CFilter_Test* CFilter_Test::getNewInstance()
{
    CFilter_Test *tmp = new CFilter_Test;
    if(tmp != NULL)
    {
        return tmp;
    }
    else
    {
        return NULL;
    }
}

int filter_test_get_interface(void **ppIntf){
	if(ppIntf == NULL)
		return -1;

	*ppIntf = new CFilter_Test;
	return *ppIntf != NULL;
}



