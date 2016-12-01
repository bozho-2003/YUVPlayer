
#include "format_i420.h"



#define YUV_FORMATER_ID    0x1
#define YUV_FORMATER_NAME  "I420(yuv420)"

#define MODULE_VERSION    "0.0.1"
#define MODULE_AUTHOR     "netjunegg@gmail.com"
#define MODULE_COMMENT    "i420, yuv, 4:1:1"


void CFormater_I420::_init(){
    m_buf = NULL;
    m_bufsz = 0;

    m_block_size = 0;

    m_frame_num = 0;
    m_index = -1;

    m_width = m_height = 0;
    m_field = FIELD_UNKNOWN;
    m_y_size = 0;
}
CFormater_I420::CFormater_I420(){
    _init();
}
CFormater_I420::~CFormater_I420(){
    destroy();
}

// format name
const char * CFormater_I420::getName(){
    return YUV_FORMATER_NAME;
}
// other information
int  CFormater_I420::getModuleInfo(Module_Info *pInfo){
    sprintf(pInfo->author_name,MODULE_AUTHOR);
    sprintf(pInfo->comment,MODULE_COMMENT);
    sprintf(pInfo->version,MODULE_VERSION);
    return E_OK;
}

int  CFormater_I420::simpleConfig(int val, unsigned int val2, const char *str){
    return 0;
}
int  CFormater_I420::selfConfig(){
    return 0;
}

int  CFormater_I420::queryRuntimeInfo(char *infoBuf, int bufsz){
    return 0;
}

int  CFormater_I420::setParams(YUV_Params *params){
    if(params == NULL)
        return E_BAD_ARG;
    if(params->width <= 0 || params->height <= 0
        || (params->width&1) || (params->height&1) )
        return E_UNSUPPORTTED;
    if(params->field != FIELD_ALL && params->field != FIELD_TOP_ONLY
        && params->field != FIELD_BOTTOM_ONLY && params->field != FIELD_TOP_BOTTOM_SEQ)
        return E_UNSUPPORTTED;

    m_width = params->width;
    m_height = params->height;
    m_field = params->field;

    m_y_size = m_width*m_height;
    m_block_size = m_width*m_height*3/2;
    if(m_field == FIELD_TOP_BOTTOM_SEQ)
        m_frame_num = 2;
    else
        m_frame_num = 1;
    return E_OK;
}

int  CFormater_I420::update(void *pErr){
    if(m_bufsz < m_block_size){
        if(m_buf)
            free(m_buf);
        m_bufsz = 0;
        m_buf = (unsigned char *)malloc(m_block_size);
        if(m_buf == NULL)
            return E_NO_MEMORY;
        m_bufsz = m_block_size;
    }

    return 1;
}

int  CFormater_I420::getOutInfo(RawImage_Info *out){
    if(out == NULL)
        return E_BAD_ARG;
    out->raw_format = RAW_FORMAT_YUV444;
    if(m_field == FIELD_ALL){
        out->height = m_height;
    }
    else{
        out->height = m_height/2;
    }
    out->width = m_width;
    return E_OK;
}

int  CFormater_I420::getMinBlockSize(){
    return m_block_size;
}
int  CFormater_I420::getMinBlockFrameNum(){
    return m_frame_num;
}

int  CFormater_I420::getBuffer(unsigned char **ppBuf){
    if(ppBuf == NULL)
        return E_BAD_ARG;
    *ppBuf = NULL;
    if(m_block_size <= 0 ||m_bufsz < m_block_size)
        return E_NO_INIT;

    *ppBuf = m_buf;
    return E_OK;
}
int  CFormater_I420::putData(unsigned char *pBuf){
    if(pBuf != m_buf)
        return E_BAD_ARG;
    m_index = 0;
    return E_OK;
}

int  CFormater_I420::getFrameIndex(){
    return m_index;
}
int  CFormater_I420::setFrameIndex(int index){
    if(index < 0 || index > m_frame_num)
        return E_BAD_ARG;
    m_index = index;
    return E_OK;
}

int  CFormater_I420::getFrame(int offset, unsigned char *pBuf, int bufsz){
    int index = m_index;
    if(index < 0)
        return E_NO_INIT;
    if(offset > 0) index++;
    else if(offset < 0) index--;

    if(index < 0 || index > m_frame_num)
        return 0;
    // added!!!
    if(index == 0 && m_field == FIELD_BOTTOM_ONLY)
        index = 1;

    int n;
    if(m_field == FIELD_ALL){  // all field
        unsigned char *p1,*p2;
        int i,j;

        // copy Y
        memcpy(pBuf, m_buf, m_y_size);

        // copy U
        p1 = m_buf + m_y_size;
        p2 = pBuf + m_y_size;
        for(i = 0;i < m_height/2;i++){
            for(j = 0;j < m_width/2;j++){
                // pixel dup
                *p2++ = *p1;
                *p2++ =*p1++;
            }
            // line dup
            p1 -= m_width/2;
            for(j = 0;j < m_width/2;j++){
                *p2++ = *p1;
                *p2++ =*p1++;
            }
        }

        // copy V
        p1 = m_buf + m_y_size + m_y_size/4;
        p2 = pBuf + m_y_size + m_y_size;
        for(i = 0;i < m_height/2;i++){
            for(j = 0;j < m_width/2;j++){
                // pixel dup
                *p2++ = *p1;
                *p2++ =*p1++;
            }
            // line dup
            p1 -= m_width/2;
            for(j = 0;j < m_width/2;j++){
                *p2++ = *p1;
                *p2++ =*p1++;
            }
        }
    }
    else if(index == 0){   // top field
        unsigned char *p1,*p2;
        int i,j;

        // copy Y
        p1 = m_buf;
        p2 = pBuf;
        for(i = 0;i < m_height/2;i++){
            for(j = 0;j < m_width;j++){
                *p2++ = *p1++;
            }
            p1 += m_width;
        }

        // copy U
        p1 = m_buf + m_y_size;
        p2 = pBuf + m_y_size/2;
        for(i = 0;i < m_height/2;i++){
            for(j = 0;j < m_width/2;j++){
                *p2++ = *p1;
                *p2++ =*p1++;
            }
        }

        // copy V
        p1 = m_buf + m_y_size + m_y_size/4;
        p2 = pBuf + m_y_size;
        for(i = 0;i < m_height/2;i++){
            for(j = 0;j < m_width/2;j++){
                *p2++ = *p1;
                *p2++ =*p1++;
            }
        }
    }
    else if(index == 1){
        unsigned char *p1,*p2;
        int i,j;

        // copy Y
        p1 = m_buf;
        p2 = pBuf;
        p1 += m_width;
        for(i = 0;i < m_height/2;i++){
            for(j = 0;j < m_width;j++){
                *p2++ = *p1++;
            }
            p1 += m_width;
        }

        // copy U
        p1 = m_buf + m_y_size;
        p2 = pBuf + m_y_size/2;
        for(i = 0;i < m_height/2;i++){
            for(j = 0;j < m_width/2;j++){
                *p2++ = *p1;
                *p2++ =*p1++;
            }
        }
        // copy V
        p1 = m_buf + m_y_size + m_y_size/4;
        p2 = pBuf + m_y_size;
        for(i = 0;i < m_height/2;i++){
            for(j = 0;j < m_width/2;j++){
                *p2++ = *p1;
                *p2++ =*p1++;
            }
        }
    }
    else
        return E_OOPS;

    return index;
}

int  CFormater_I420::toYourFormat(RawImage_Info *src_info, const unsigned char *src_buf, int src_bufsz, unsigned char *dst_buf, int *p_dst_bufsz){
    if(src_info == NULL)
        return -1;

    if(src_info->raw_format != RAW_FORMAT_YUV444)
        return -2;

    int outsize = src_info->width*src_info->height*3/2;
    if(outsize <= 0)
        return -3;

    if(dst_buf == NULL){
        if(p_dst_bufsz == NULL)
            return -4;
        *p_dst_bufsz = outsize;
        return E_OK;
    }

    if(src_buf == NULL || dst_buf == NULL)
        return -5;

    int width = src_info->width;
    int height = src_info->height;
    int y_size = width*height;
    const unsigned char *p1;
    unsigned char *p2;
    int i,j;

    // Y
    p1 = src_buf;
    p2 = dst_buf;
    memcpy(p2, p1, y_size);

    // U
    p1 = src_buf + y_size;
    p2 = dst_buf + y_size;
    for(i = 0;i < height/2;i++){
        for(j = 0;j < width/2;j++){
            *p2++ = *p1;
            p1 += 2;
        }
        p1 += width;
    }

    // U
    p1 = src_buf + y_size*2;
    p2 = dst_buf + y_size + y_size/4;
    for(i = 0;i < height/2;i++){
        for(j = 0;j < width/2;j++){
            *p2++ = *p1;
            p1 += 2;
        }
        p1 += width;
    }

    return 1;
}

int  CFormater_I420::destroy(){
    if(m_buf)
        free(m_buf);
    _init();
    return 1;
}


int formater_i420_get_interface(void **ppIntf){
    if(ppIntf == NULL)
        return -1;
    *ppIntf = new CFormater_I420;
    return (ppIntf != NULL);
}

