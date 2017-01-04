#include <QImage>
#include <QDebug>
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
    "        <val value= \"%s\"/>"
    "        <modflag value = \"1\"/>"
    "        <visibility value = \"0\"/>"
    "        <showname><![CDATA[kernel matrix]]></showname>"
    "        <comment><![CDATA[0,255]]]></comment>"
    "      </parainfo>"
    "    </label>"
    "  </para>"
    "</filterinfo>";


CFilter_ConvFilter::CFilter_ConvFilter()
{
	m_frame_size = 0;
    m_line_extra_bytes = 0;
    m_img_out = NULL;

    m_kernel_w = 3;
    m_kernel_h = 3;
    m_kernel_size = 9;
    strcpy(m_kernel_str, "0 1 0 1 2 1 0 1 0");
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

    //qDebug() << infoBuf;
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
                        if (strcmp(type, "str") == 0)
                        {
                            TXMLElePtr tVal = XMLGetFirstChildElement(tParaInfo, "val");
                            TXMLElePtr tVisibility = XMLGetFirstChildElement(tParaInfo, "visibility");
                            TXMLElePtr tName = XMLGetFirstChildElement(tParaInfo, "showname");
                            float min, max, val, visibility_flag;
                            //const char *val_str = XMLGetElementText(tVal);
                            const char *val_str;
                            val_str = XMLGetElementAttributes8(tVal, "value");
                            XMLGetElementAttributef32(tVisibility, "value", &visibility_flag);
                            const char *name_str = XMLGetElementText(tName);
                            if(strcmp(name, "kernel") == 0)
                            {
                                //copy to str buffer
                                char kernel_str[1024];
                                strcpy(m_kernel_str, val_str);
                                strcpy(kernel_str, val_str);
                                //Parse string to kernel matrix
                                char *number_str[256];
                                int str_len = strlen(kernel_str);
                                for(int i = 0; i < str_len; i++)
                                {
                                    if(kernel_str[i] == ' ')
                                    {
                                        kernel_str[i] = '\0';
                                    }
                                }

                                int j = 0;
                                for(int i = 0; i < str_len; i++)
                                {
                                    if(kernel_str[i] != '\0')
                                    {
                                        number_str[j++] = kernel_str + i;
                                        i += strlen(kernel_str + i);
                                        continue;
                                    }
                                }
                                for(int i = 0; i < j; i++)
                                {
                                    m_kernel_val[i] = QString(number_str[i]).toInt();
                                }

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
                            if(strcmp(name, "k-w") == 0)
                            {
                                m_kernel_w = val;
                            }
                            else if(strcmp(name, "k-h") == 0)
                            {
                                m_kernel_h = val;
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

    sprintf(infoBuf, CFilter_ConvFilter::parameter_format,
            m_kernel_w,
            m_kernel_h,
            m_kernel_str);


    //qDebug() << infoBuf;
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
        m_img_out = new unsigned char[m_frame_size];
    }
    else if(m_inInfo.raw_format == RAW_FORMAT_YUV444 ){
        m_frame_size = m_inInfo.width*m_inInfo.height;
        m_off1 = 0;
        m_off2 = m_frame_size;
        m_off3 = m_frame_size*2;
        m_frame_size *= 3;
        m_line_extra_bytes = 0;
        m_img_out = new unsigned char[m_frame_size];
    }
	else if(m_inInfo.raw_format == RAW_FORMAT_RGB24){
		m_frame_size = m_inInfo.width*m_inInfo.height;
		m_off1 = 0;
		m_off2 = 1;
		m_off3 = 2;
		m_frame_size *= 3;
		m_line_extra_bytes = 0;
        m_img_out = new unsigned char[m_frame_size];
	}
	else if(m_inInfo.raw_format == RAW_FORMAT_BGR24){
		m_line_extra_bytes = m_inInfo.width*3;
		while(m_line_extra_bytes&0x3) m_line_extra_bytes++;
		m_frame_size = m_line_extra_bytes*m_inInfo.height;
		m_line_extra_bytes -= m_inInfo.width*3;
		m_off1 = 2;
		m_off2 = 1;
		m_off3 = 0;
        m_img_out = new unsigned char[m_frame_size];
	}
	else if(m_inInfo.raw_format == RAW_FORMAT_GREY8){
		m_frame_size = m_inInfo.width*m_inInfo.height;
		m_off1 = 0;
		m_off2 = -1;
		m_off3 = -1;
		m_line_extra_bytes = 0;
        m_img_out = new unsigned char[m_frame_size];
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

int  CFilter_ConvFilter::conv_filter_pixel(unsigned char *p1, unsigned char *p1_dst, int row, int column)
{
    long pixel_weight_sum = 0;
    long weight_sum = 0;

    int i = row;
    int j = column;

    int k_i, k_j;
    for(k_i = 0; k_i < m_kernel_h; k_i++)
    {
        for(k_j = 0; k_j < m_kernel_w; k_j++)
        {
            int idx_i, idx_j;
            idx_i = i - m_kernel_h / 2 + k_i;
            idx_j = j - m_kernel_w / 2 + k_j;

            if(idx_i < 0) idx_i = 0;
            if(idx_i >= m_inInfo.height) idx_i = m_inInfo.height - 1;

            if(idx_j < 0) idx_j = 0;
            if(idx_j >= m_inInfo.width) idx_j = m_inInfo.width - 1;

            long weight = m_kernel_val[k_i * m_kernel_w + k_j];
            pixel_weight_sum += p1[idx_i * m_inInfo.width + idx_j] * weight;
            weight_sum += weight;
        }
    }

    unsigned long tmp;
    if(weight_sum != 0)
    {
        tmp = pixel_weight_sum / weight_sum;
    }
    else
    {
        tmp = pixel_weight_sum;
    }

    if(tmp > 255) tmp = 255;
    if(tmp < 0) tmp = 0;
    p1_dst[i * m_inInfo.width + j] = tmp;
}

int  CFilter_ConvFilter::conv_filter_444(unsigned char **ppData, int *pLen)
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

    unsigned char *p1_dst,*p2_dst,*p3_dst;
    p1_dst = m_img_out + m_off1;
    p2_dst = m_img_out + m_off2;
    p3_dst = m_img_out + m_off3;

    for(i = 0;i < m_inInfo.height;i++){
        for(j = 0;j < m_inInfo.width;j++)
        {
            unsigned long tmp;
            if(m_off1 >= 0)
            {
                conv_filter_pixel(p1, p1_dst, i, j);
            }
            if(m_off2 >= 0)
            {
                p2_dst[i * m_inInfo.width + j] = p2[i * m_inInfo.width + j];
            }
            if(m_off3 >= 0)
            {
                p3_dst[i * m_inInfo.width + j] = p3[i * m_inInfo.width + j];
            }
        }
    }

    return 1;
}


int  CFilter_ConvFilter::conv_filter_yuv420(unsigned char **ppData, int *pLen)
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

    unsigned char *p1_dst,*p2_dst,*p3_dst;
    p1_dst = m_img_out + m_off1;
    p2_dst = m_img_out + m_off2;
    p3_dst = m_img_out + m_off3;

    for(i = 0;i < m_inInfo.height;i++)
    {
        for(j = 0;j < m_inInfo.width;j++)
        {

            if(m_off1 >= 0)
            {
                conv_filter_pixel(p1, p1_dst, i, j);
            }

        }
    }

    for(i = 0;i < (m_inInfo.height / 2);i++)
    {
        for(j = 0;j < (m_inInfo.width / 2);j++)
        {
            if(m_off2 >= 0)
            {
                p2_dst[i * (m_inInfo.width / 2) + j] = p2[i * (m_inInfo.width / 2) + j];
            }
            if(m_off3 >= 0)
            {
                p3_dst[i * (m_inInfo.width / 2) + j] = p3[i * (m_inInfo.width / 2) + j];
            }
        }
    }

    *ppData = m_img_out;
    *pLen = m_frame_size;
    return 1;
}

// filter
int  CFilter_ConvFilter::filter(unsigned char **ppData, int *pLen)
{
    if(m_inInfo.raw_format == RAW_FORMAT_YUV420 )
    {
        return conv_filter_yuv420(ppData, pLen);
    }
    else if((m_inInfo.raw_format == RAW_FORMAT_YUV444) ||
            (m_inInfo.raw_format == RAW_FORMAT_RGB24) ||
            (m_inInfo.raw_format == RAW_FORMAT_BGR24) ||
            (m_inInfo.raw_format == RAW_FORMAT_GREY8))
    {
        return conv_filter_444(ppData, pLen);
    }

    return 1;
}


int  CFilter_ConvFilter::destroy(){
    if(m_img_out != NULL)
        delete[] m_img_out;
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



