#include "render.h"

void CRender::_init(){
    m_inBuf = NULL;
    m_inBufSz = 0;

    m_frameSize = 0;
    m_showYUV = YUV_SHOW_YUV;

    m_ppFilters = NULL;

    m_rawBuf = NULL;
    m_rawBufSz = 0;
    m_imageBuf = NULL;
    m_imageBufSz = 0;

    m_rawInfo.raw_format = RAW_FORMAT_UNKNOWN;

    m_bValid = 0;
}
CRender::CRender(){
    _init();
}

CRender::~CRender(){
    destroy();
}

int CRender::_checkFilters(int *pErr){
    RawImage_Info last_info = m_inInfo;
    IImage_Filter *pFilter;
    int i;
    int num = 0;

    for(i = 0;;i++){
        if(m_ppFilters[i] == NULL)
            break;

        int ret;

        ret = m_ppFilters[i]->setParams(&last_info);
        if(ret < 0){
            if(pErr) *pErr = i;
            m_errMsg = QString("image filter \"%1\" doesn't support the given params, %2").arg(m_ppFilters[i]->getName(), ret);
            return -2;
        }

        int err;
        ret = m_ppFilters[i]->update(&err);
        if(ret < 0){
            if(pErr) *pErr = i;
            m_errMsg = QString("image filter \"%1\" update failed, %2").arg(m_ppFilters[i]->getName(), ret);
            return -3;
        }

        m_ppFilters[i]->getOutInfo(&last_info);
        num++;
    }

    m_rawInfo = last_info;
    m_filterNum = num;
    return num;
}

// 1
int CRender::_calcFrameSize(RawImage_Info *pInfo){
    if(pInfo == NULL)
        return -1;
    if(pInfo->width <= 0 || pInfo->height <= 0){
        m_errMsg = QString("bad resolution: %1x%2").arg(pInfo->width, pInfo->height);
        return -2;
    }

    if(pInfo->raw_format == RAW_FORMAT_YUV444){
        pInfo->frame_size = pInfo->width*pInfo->height*3;

        // pixel info
        m_linePitch[0] = pInfo->width;
        m_linePitch[1] = pInfo->width;
        m_linePitch[2] = pInfo->width;
        m_offs[0] = 0;
        m_offs[1] = pInfo->frame_size/3;
        m_offs[2] = pInfo->frame_size/3*2;
        m_step = 1;

        m_pixelFormat = 0;
        return 1;
    }
    else if(pInfo->raw_format == RAW_FORMAT_YUV420){
        pInfo->frame_size = pInfo->width*pInfo->height*3/2;

        // pixel info
        m_linePitch[0] = pInfo->width;
        m_linePitch[1] = pInfo->width / 2;
        m_linePitch[2] = pInfo->width / 2;
        m_offs[0] = 0;
        m_offs[1] = pInfo->width*pInfo->height;
        m_offs[2] = pInfo->width*pInfo->height * 5 / 4;
        m_step = 1;

        m_pixelFormat = 0;
        return 1;
    }
    else if(pInfo->raw_format == RAW_FORMAT_RGB24){
        pInfo->frame_size = pInfo->width*pInfo->height*3;

        // pixel info
        m_linePitch[0] = pInfo->width*3;
        m_linePitch[1] = pInfo->width*3;
        m_linePitch[2] = pInfo->width*3;
        m_offs[0] = 0;
        m_offs[1] = 1;
        m_offs[2] = 2;
        m_step = 3;

        m_pixelFormat = 1;
        return 1;
    }
    else if(pInfo->raw_format == RAW_FORMAT_BGR24){
        int tmp = pInfo->width*3;
        while(tmp&0x3) tmp++;
        pInfo->frame_size = tmp*pInfo->height;

        // pixel info
        m_linePitch[0] = tmp;
        m_linePitch[1] = tmp;
        m_linePitch[2] = tmp;
        m_offs[0] = 2;
        m_offs[1] = 1;
        m_offs[2] = 0;
        m_step = 3;

        m_pixelFormat = 1;
        return 1;
    }
    else if(pInfo->raw_format == RAW_FORMAT_GREY8){
        pInfo->frame_size = pInfo->width*pInfo->height;

        m_linePitch[0] = pInfo->width;
        m_linePitch[1] = -1;
        m_linePitch[2] = -1;
        m_offs[0] = 0;
        m_offs[1] = -1;
        m_offs[2] = -1;
        m_step = 1;

        m_pixelFormat = 2;
        return 1;
    }
    else if(pInfo->frame_size > 0){
        return 0;
    }

    m_errMsg = "a custom format need valid frame size";
    return -3;
}

int CRender::setShowYUV(int showYUV){
    if(showYUV != YUV_SHOW_YUV
        && showYUV != YUV_SHOW_Y
        && showYUV != YUV_SHOW_U
        && showYUV != YUV_SHOW_V)
        return -1;

    m_showYUV = showYUV;
    return 1;
}

int CRender::setParams(RawImage_Info *pInfo){
    if(pInfo == NULL)
        return -1;
    m_inInfo = *pInfo;
    return 1;
}

int CRender::setFilters(IImage_Filter **ppFilters){
    m_ppFilters = ppFilters;
    return 1;
}

int CRender::update(void *pErr){
    int format = _calcFrameSize(&m_inInfo);

    if(format < 0)
        return -2;

    if(m_ppFilters){
        int err;
        if(_checkFilters(&err) < 0){
            return -2;
        }
    }
    else{
        m_filterNum = 0;
        m_rawInfo = m_inInfo;
    }

    if(format == 0 && m_filterNum <= 0){
        m_errMsg  = QString("unsupportted format from formater plugin, maybe you need image filter, format id: %1(0x%2)").arg(m_inInfo.raw_format, m_inInfo.raw_format);
        return -2;
    }

    if(_calcFrameSize(&m_rawInfo) <= 0){  // unknown type
        return -4;
    }

    // inbuf size
    m_frameSize = m_inInfo.frame_size;
    if(m_inBufSz < m_frameSize){
        if(m_inBuf)
            free(m_inBuf);
        m_inBuf = (unsigned char*)malloc(m_frameSize);
        if(m_inBuf == NULL){
            m_inBufSz = 0;
            return -5;
        }
        m_inBufSz = m_frameSize;
    }
    // extra
    m_rawBuf = m_inBuf;
    m_rawBufSz = m_inBufSz;

    // imagebuf size
    m_linePitch_bgr24 = m_rawInfo.width*3;
    while(m_linePitch_bgr24&0x3) m_linePitch_bgr24++;
    int tmp = m_linePitch_bgr24*m_rawInfo.height;
    if(m_imageBufSz < tmp){
        if(m_imageBuf)
            free(m_imageBuf);
        m_imageBufSz = 0;
        m_imageBuf = (unsigned char*)malloc(tmp);
        if(m_imageBuf == NULL)
            return -4;
        m_imageBufSz = tmp;
    }

    memset(&m_bmpInfo,0,sizeof(m_bmpInfo));
    m_bmpInfo.biSize = 40;
    m_bmpInfo.biWidth = m_rawInfo.width;
    m_bmpInfo.biHeight = m_rawInfo.height;
    m_bmpInfo.biBitCount = 24;
    m_bmpInfo.biSizeImage = m_imageBufSz;
    m_bmpInfo.biPlanes = 1;
    m_bmpInfo.biCompression = BI_RGB;

//	m_skipRending = 0;
    return 1;
}

int CRender::getBuffer(unsigned char **ppBuf, int *p_bufsz){
    if(ppBuf == NULL)
        return -1;
    *ppBuf = m_inBuf;
    if(p_bufsz) *p_bufsz = m_inBufSz;

    m_bValid = 0;
    return 1;
}

int CRender::putData(unsigned char *pBuf){
    if(pBuf != m_inBuf)
        return -1;

    if(m_filterNum > 0){
        unsigned char *pBuf = m_inBuf;
        int bufsz = m_frameSize;
        int i;
        int ret;
        for(i = 0;;i++){
            if(m_ppFilters[i] == NULL)
                break;
            ret = m_ppFilters[i]->filter(&pBuf,&bufsz);
            if(ret < 0){
                m_errMsg = QString("image filter failed, name: %1, ret: %2").arg(m_ppFilters[i]->getName(), ret);
                return -2;
            }
        }
        m_rawBuf = pBuf;
        m_rawBufSz = bufsz;
    }
    else{
        m_rawBuf = m_inBuf;
        m_rawBufSz = m_inBufSz;
    }

    if(m_rawInfo.raw_format == RAW_FORMAT_YUV444){
        if(m_showYUV == YUV_SHOW_YUV)
            yuv444_2_rgb24(m_rawBuf, m_imageBuf, m_rawInfo.width, m_rawInfo.height);
        else if(m_showYUV == YUV_SHOW_Y)
            grey8_2_rgb24(m_rawBuf + m_offs[0], m_imageBuf, m_rawInfo.width, m_rawInfo.height);
        else if(m_showYUV == YUV_SHOW_U)
            grey8_2_rgb24(m_rawBuf + m_offs[1], m_imageBuf, m_rawInfo.width, m_rawInfo.height);
        else if(m_showYUV == YUV_SHOW_V)
            grey8_2_rgb24(m_rawBuf + m_offs[2], m_imageBuf, m_rawInfo.width, m_rawInfo.height);
    }
    else if(m_rawInfo.raw_format == RAW_FORMAT_YUV420){
        if(m_showYUV == YUV_SHOW_YUV)
            yuv420_2_rgb24(m_rawBuf, m_imageBuf, m_rawInfo.width, m_rawInfo.height);
        else if(m_showYUV == YUV_SHOW_Y)
            grey8_2_rgb24(m_rawBuf + m_offs[0], m_imageBuf, m_rawInfo.width, m_rawInfo.height);
        else if(m_showYUV == YUV_SHOW_U)
            grey8_2_rgb24(m_rawBuf + m_offs[1], m_imageBuf, m_rawInfo.width, m_rawInfo.height);
        else if(m_showYUV == YUV_SHOW_V)
            grey8_2_rgb24(m_rawBuf + m_offs[2], m_imageBuf, m_rawInfo.width, m_rawInfo.height);
    }
    else if(m_rawInfo.raw_format == RAW_FORMAT_BGR24){
        bgr24_2_rgb24(m_rawBuf, m_imageBuf, m_rawInfo.width, m_rawInfo.height);
    }
    else if(m_rawInfo.raw_format == RAW_FORMAT_RGB24){
        rgb24_2_rgb24(m_rawBuf, m_imageBuf, m_rawInfo.width, m_rawInfo.height);
    }
    else if(m_rawInfo.raw_format == RAW_FORMAT_GREY8){
        grey8_2_rgb24(m_rawBuf, m_imageBuf, m_rawInfo.width, m_rawInfo.height);
    }
    else
        return -3;

    m_bValid = 1;
    return 1;
}

//
int CRender::getImageBmp(BITMAPINFOHEADER **ppBmpInfo, unsigned char **ppBits){
    if(m_bValid == 0)
        return -3;

    if(ppBmpInfo == NULL || ppBits == NULL)
        return -1;
    if(m_frameSize < 0 || m_inBufSz < m_frameSize)
        return -2;

    *ppBmpInfo = &m_bmpInfo;
    *ppBits = m_imageBuf;
    return 1;
}

int CRender::getRawInfo(RawImage_Info *pInfo){
    if(pInfo == NULL || m_rawInfo.raw_format == RAW_FORMAT_UNKNOWN)
        return -1;
    *pInfo = m_rawInfo;
    return 1;
}

const unsigned char* CRender::getRaw(int *pLen){
    if(m_bValid == 0)
        return NULL;

    if(pLen) *pLen = m_rawBufSz;
    return m_rawBuf;
}

int CRender::getBitCount_C(int arg){
    if(m_bValid == 0)
        return -3;
    return 8;
}

int CRender::getPixels_Y(int x, int y, int cx, unsigned char *buf, int bufsz){
    if(m_bValid == 0)
        return -3;

    if(m_offs[0] < 0)
        return 0;
    if(x < 0 || y < 0 || cx <= 0 || buf == NULL || bufsz <= 0)
        return -1;
    if(x >= m_rawInfo.width || y >= m_rawInfo.height)
        return -1;
    if(cx > m_rawInfo.width - x)
        cx = m_rawInfo.width - x;
    if(cx <= 0 || bufsz < cx)
        return -1;

    unsigned char *p = m_rawBuf;
    p += m_offs[0] + y*m_linePitch[0] + x*m_step;

    for(int i = 0;i < cx;i++){
        buf[i] = *p;
        p += m_step;
    }

    return cx;
}

QString CRender::getPixels_YUV_str(int x, int y, int nLine, int numPixels, int yuv_flag, int valueMode)
{
    unsigned char  buf[1024];
    int i;
    int x_start, y_start;
    QString text, buf2;
    x_start = x;
    y_start = y;

    if((yuv_flag != 0) && (m_rawInfo.raw_format == RAW_FORMAT_YUV420))
    {
        x_start = x / 2;
        y_start = y / 2;
        nLine = nLine / 2;
        numPixels = numPixels / 2;
    }

    for(i = 0;i < nLine;i++){
        int nBytes;
        const  unsigned char *p1;

        // return actual size
        if (yuv_flag == 0)
        {
            nBytes = getPixels_Y(x_start,y_start+i,numPixels,(unsigned char*)buf,numPixels);
        }
        else if (yuv_flag == 1)
        {
            nBytes = getPixels_U(x_start,y_start+i,numPixels,(unsigned char*)buf,numPixels);
        }
        else if (yuv_flag == 2)
        {
            nBytes = getPixels_V(x_start,y_start+i,numPixels,(unsigned char*)buf,numPixels);
        }

        if(nBytes <= 0)
            break;

        p1 = buf;
        while(nBytes-- > 0){
            if(valueMode == VALUE_MODE_DEC){
                buf2 = QString().sprintf("%.3d ",*(const unsigned char*)p1);
            }
            else{
                buf2 = QString().sprintf("%.2X ",*(const unsigned char*)p1);
            }

            text += buf2;
            p1++;
        }
        text += "\r\n";
    }
    return text;
}

int CRender::getPixels_U(int x, int y, int cx, unsigned char *buf, int bufsz){
    if(m_bValid == 0)
        return -3;

    if(m_offs[1] < 0)
        return 0;
    if(x < 0 || y < 0 || cx <= 0 || buf == NULL || bufsz <= 0)
        return -1;
    if(x >= m_rawInfo.width || y >= m_rawInfo.height)
        return -1;
    if(cx > m_rawInfo.width - x)
        cx = m_rawInfo.width - x;
    if(cx <= 0 || bufsz < cx)
        return -1;

    unsigned char *p = m_rawBuf;

    p += m_offs[1] + y*m_linePitch[1] + x*m_step;

    for(int i = 0;i < cx;i++){
        buf[i] = *p;
        p += m_step;
    }

    return cx;
}
int CRender::getPixels_V(int x, int y, int cx, unsigned char *buf, int bufsz){
    if(m_bValid == 0)
        return -3;

    if(m_offs[2] < 0)
        return 0;
    if(x < 0 || y < 0 || cx <= 0 || buf == NULL || bufsz <= 0)
        return -1;
    if(x >= m_rawInfo.width || y >= m_rawInfo.height)
        return -1;
    if(cx > m_rawInfo.width - x)
        cx = m_rawInfo.width - x;
    if(cx <= 0 || bufsz < cx)
        return -1;

    unsigned char *p = m_rawBuf;
    p += m_offs[2] + y*m_linePitch[2] + x*m_step;

    for(int i = 0;i < cx;i++){
        buf[i] = *p;
        p += m_step;
    }
    return cx;
}

int CRender::destroy(){
    if(m_inBuf)
        free(m_inBuf);
    if(m_imageBuf)
        free(m_imageBuf);

    _init();
    return 0;
}

int CRender::yuv420_2_rgb24(unsigned char *yuv_buf, unsigned char *bgr_buf, int width, int height){
    int i,j;
    int linePitch_bgr;
    unsigned char *pY,*pU,*pV;
    unsigned char *pB;

    i = width*height;
    pY = yuv_buf;
    pU = pY + i;
    pV = pU + i / 4;

    i = width*3;
    linePitch_bgr = i;
    while(linePitch_bgr&3) linePitch_bgr++;
    pB = bgr_buf;
    //pB += linePitch_bgr*(height-1);
    //linePitch_bgr += i;

    for(i = 0; i < height; i++)
    {
        for(j = 0;j < width;j++)
        {
            unsigned char r_val, g_val, b_val;
            yuv2rgb(*(pY+j), *(pU+(j>>1)), *(pV+(j>>1)), &r_val, &g_val, &b_val);

            *(pB + 3 * j) = r_val;
            *(pB + 3 * j + 1) = g_val;
            *(pB + 3 * j + 2) = b_val;
        }
        pY += width;
        pU += (width / 2) * (i & 1);
        pV += (width / 2) * (i & 1);
        pB += linePitch_bgr;
    }
    return 1;
}

int CRender::yuv444_2_rgb24(unsigned char *yuv_buf, unsigned char *bgr_buf, int width, int height){
    int i,j;
    int linePitch_bgr;
    unsigned char *pY,*pU,*pV;
    unsigned char *pB;

    i = width*height;
    pY = yuv_buf;
    pU = pY + i;
    pV = pU + i;

    i = width*3;
    linePitch_bgr = i;
    while(linePitch_bgr&3) linePitch_bgr++;
    pB = bgr_buf;
    //pB += linePitch_bgr*(height-1);
    //linePitch_bgr += i;

    for(i = 0;i < height;i++){
        for(j = 0;j < width;j++){
            unsigned char r_val, g_val, b_val;
            yuv2rgb(*pY,*pU,*pV, &r_val, &g_val, &b_val);
            pY++;
            pU++;
            pV++;
            *(pB + 3 * j) = r_val;
            *(pB + 3 * j + 1) = g_val;
            *(pB + 3 * j + 2) = b_val;
        }
        pB += linePitch_bgr;
    }
    return 1;
}

int CRender::grey8_2_rgb24(unsigned char *yuv_buf, unsigned char *bgr_buf, int width, int height){
    int i,j;
    int linePitch_bgr;
    unsigned char *pY,*pU,*pV;
    unsigned char *pB;

    pY = yuv_buf;

    i = width*3;
    linePitch_bgr = i;
    while(linePitch_bgr&3) linePitch_bgr++;
    pB = bgr_buf;
    //pB += linePitch_bgr*(height-1);
    //linePitch_bgr += i;

    for(i = 0;i < height;i++){
        for(j = 0;j < width;j++){
            *(pB + 3 * j) = *pY;
            *(pB + 3 * j + 1) = *pY;
            *(pB + 3 * j + 2) = *pY++;

        }
        pB += linePitch_bgr;
    }
    return 1;
}

int CRender::rgb24_2_rgb24(unsigned char *yuv_buf, unsigned char *bgr_buf, int width, int height){
    int frame_size = width*3;
    while(frame_size&0x3) frame_size++;
    frame_size *= height;

    memcpy(bgr_buf, yuv_buf, frame_size);
    return 1;

}
int CRender::bgr24_2_rgb24(unsigned char *yuv_buf, unsigned char *bgr_buf, int width, int height){
    int i,j;
    int linePitch_bgr;
    unsigned char *p1,*p2;

    p1 = yuv_buf;

    i = width*3;
    linePitch_bgr = i;
    while(linePitch_bgr&3) linePitch_bgr++;
    p2 = bgr_buf;
    //p2 += linePitch_bgr*(height-1);
    //linePitch_bgr += i;

    for(i = 0;i < height;i++){
        for(j = 0;j < width;j++){
            *(p2 + 3 * j) = p1[2];
            *(p2 + 3 * j + 1) = p1[1];
            *(p2 + 3 * j + 2) = p1[0];

            p1 += 3;
        }
        p2 += linePitch_bgr;
    }
    return 1;
}

const QString CRender::getErrMsg(){
    if(m_errMsg.isEmpty())
        return QString("unexpected error");
    return m_errMsg;
}

#define CAL_KERNEL(threshold, tmp)\
if (compare_type == 0)\
{\
    tmp -= threshold;\
    if(tmp < 0) tmp = 0;\
}


#define CAL_OUTER(p4, diff_val)\
if (compare_type == 0)\
{\
    if(diff_val > 0) diff_val = 1;\
    *p4++ = 0x100 - diff_val;\
}\
else\
{\
    if(diff_val > 255) diff_val = 255;\
    *p4++ = diff_val;\
}

int CRender::yuvDiff_bi_all(CRender *pref, const unsigned char *pSrc1, const unsigned char *pSrc2, int threshold, int compare_type)
{
    int diff_num = 0;
    unsigned char *p4 = m_rawBuf;
    RawImage_Info info;

    pref->getRawInfo(&info);

    if(info.raw_format != RAW_FORMAT_GREY8){
        const unsigned char *p1[3],*p2[3];
        int line_extra;
        int i,j;

        if(info.raw_format == RAW_FORMAT_BGR24){
            i = info.width * 3;
            line_extra = i;
            while(line_extra&0x3) line_extra++;
            line_extra -= i;
        }
        else
            line_extra = 0;

        for(i = 0;i < 3;i++)
        {
            p1[i] = pSrc1 + pref->m_offs[i];
            p2[i] = pSrc2 + pref->m_offs[i];
        }

        if(info.raw_format == RAW_FORMAT_YUV420)
        {
            for(i = 0;i < info.height;i++)
            {
                for(j = 0;j < info.width;j++)
                {
                    int diff_val = 0;
                    int k;
                    register int tmp;
                    tmp = abs(p1[0][j] - p2[0][j]);
                    CAL_KERNEL(threshold, tmp);
                    diff_val += tmp;

                    tmp = abs(p1[1][j>>1] - p2[1][j>>1]);
                    CAL_KERNEL(threshold, tmp);
                    diff_val += tmp;

                    tmp = abs(p1[2][j>>1] - p2[2][j>>1]);
                    CAL_KERNEL(threshold, tmp);
                    diff_val += tmp;

                    CAL_OUTER(p4, diff_val);

                    diff_num += diff_val;
                }

                p1[0] += info.width;
                p2[0] += info.width;

                p1[1] += (info.width >> 1) * (i & 1);
                p2[1] += (info.width >> 1) * (i & 1);

                p1[2] += (info.width >> 1) * (i & 1);
                p2[2] += (info.width >> 1) * (i & 1);

            }
        }
        else if(info.raw_format == RAW_FORMAT_YUV444)
        {
            for(i = 0;i < info.height;i++){
                for(j = 0;j < info.width;j++){
                    int diff_val = 0;
                    int k;
                    for(k = 0;k < 3;k++){
                        register int tmp;
                        tmp = abs(*p1[k] - *p2[k]);
                        CAL_KERNEL(threshold, tmp);
                        diff_val += tmp;
                    }

                    CAL_OUTER(p4, diff_val);

                    diff_num += diff_val;

                    for(k = 0;k < 3;k++){
                        p1[k] += pref->m_step;
                        p2[k] += pref->m_step;
                    }
                }

                if(line_extra > 0){
                    for(int k = 0;k < 3;k++){
                        p1[k] += line_extra;
                        p2[k] += line_extra;
                    }
                }
            }
        }
    }
    else{  // gray8
        const unsigned char *p1,*p2;
        unsigned char *p3;
        int num = info.width*info.height;
        int i;
        int width = info.width;

        p1 = pSrc1;
        p2 = pSrc2;
        i = 0;
        while(i++ < num)
        {
            int diff_val;
            diff_val = abs(*p1 - *p2);

            CAL_KERNEL(threshold, diff_val);
            CAL_OUTER(p4, diff_val);

            diff_num += diff_val;

            p1++;
            p2++;
        }
    }

    return diff_num;
}

int CRender::yuvDiff_bi_comp(CRender *pref, const unsigned char *pSrc1, const unsigned char *pSrc2, int threshold, int compare_type, int comp_id)
{
    int diff_num = 0;
    unsigned char *p4 = m_rawBuf;
    int index = comp_id;
    RawImage_Info info;

    pref->getRawInfo(&info);
    if(index > 2)
        return -1;

    if(info.raw_format != RAW_FORMAT_GREY8){
        const unsigned char *p1[3],*p2[3];
        int line_extra;
        int i,j;

        if(info.raw_format == RAW_FORMAT_BGR24){
            i = info.width*3;
            line_extra = i;
            while(line_extra&0x3) line_extra++;
            line_extra -= i;
        }
        else
            line_extra = 0;

        if(info.raw_format == RAW_FORMAT_YUV420)
        {
            p1[index] = pSrc1 + pref->m_offs[index];
            p2[index] = pSrc2 + pref->m_offs[index];

            for(i = 0;i < info.height;i++){
                for(j = 0;j < info.width;j++)
                {
                    int diff_val = 0;
                    register int tmp;

                    if(index == 0)
                    {
                        tmp = abs(p1[index][j] - p2[index][j]);
                        CAL_KERNEL(threshold, tmp);
                        diff_val += tmp;
                    }
                    else
                    {
                        tmp = abs(p1[index][j>>1] - p2[index][j>>1]);
                        CAL_KERNEL(threshold, tmp);
                        diff_val += tmp;
                    }

                    CAL_OUTER(p4, diff_val);

                    diff_num += diff_val;
                }

                if(index == 0)
                {
                    p1[index] += info.width;
                    p2[index] += info.width;
                }
                else
                {
                    p1[index] += (info.width >> 1) * (i & 1);
                    p2[index] += (info.width >> 1) * (i & 1);
                }

                if(line_extra > 0){
                    p1[index] += line_extra;
                    p2[index] += line_extra;
                }

            }
        }
        else if(info.raw_format == RAW_FORMAT_YUV444)
        {
            p1[index] = pSrc1 + pref->m_offs[index];
            p2[index] = pSrc2 + pref->m_offs[index];

            for(i = 0;i < info.height;i++){
                for(j = 0;j < info.width;j++)
                {
                    int diff_val = 0;
                    register int tmp;

                    tmp = abs(p1[index][j] - p2[index][j]);
                    CAL_KERNEL(threshold, tmp);
                    diff_val += tmp;

                    CAL_OUTER(p4, diff_val);
                    diff_num += diff_val;
                }
                p1[index] += info.width;
                p2[index] += info.width;

                if(line_extra > 0){
                    p1[index] += line_extra;
                    p2[index] += line_extra;
                }

            }

        }
    }
    else{  // gray8
        const unsigned char *p1,*p2;
        unsigned char *p3;
        int num = info.width*info.height;
        int i;
        int width = info.width;

        p1 = pSrc1;
        p2 = pSrc2;
        i = 0;
        while(i++ < num){
            int diff_val;
            diff_val = abs(*p1 - *p2);

            CAL_KERNEL(threshold, diff_val);

            CAL_OUTER(p4, diff_val);

            diff_num += diff_val;

            p1++;
            p2++;
        }
    }

    return diff_num;
}


const char *getRawFormatName(int format){
    if(format == RAW_FORMAT_YUV444)
        return "YUV444";
    else if(format == RAW_FORMAT_RGB24)
        return "RGB24";
    else if(format == RAW_FORMAT_BGR24)
        return "BGR24";
    else if(format == RAW_FORMAT_GREY8)
        return "GREY8";
    else
        return "unknown format";
}

