#include "common.h"


//
//  RBG: RGB16,RGB24, 分别为: 2,3
//  YUV: YUV422,YUV420,Y only, 分别为: 1,2
//
int myYUVToRGB(unsigned char *pYuv,unsigned char *pRgb,
               int width,int height,int yuvType,int rgbType){
    unsigned char *py,*pu,*pv;
    unsigned char y,u,v;
    int bt,gt,rt;
    char ys,us,vs;
    int yuv_left;	// 点数,非字节数
    int yuv_type;
    int n_point;

    unsigned char *pb;
    char bs;
    int rgb_type;
    int rgb_line_pad;
    int rgb_line_pitch;

    int topset,leftset;

    int tmp;
    int i,j;

    //
    yuv_type = yuvType;	// 1:422,2:420, 4,8
    py = pYuv;
    if(yuv_type == 1){	// YUV422P
        pu = py + width*height;
        pv = pu + width*height/2;
        ys = 1;
        us = vs = 1;
    }
    else if(yuv_type == 2){	// YUV420P
        pu = py + width*height;
        pv = pu + width*height/4;
        ys = 1;
        us = vs = 1;
    }
    else if(yuv_type == 1*4){	// YUV422 packed
        pu = py + 1;
        pv = pu + 2;
        ys = 2;
        us = vs = 4;
        yuv_type >>= 2;
    }
    else{		// 暂不支持
        return -1;
    }
    yuv_left = 0;

    rgb_type = rgbType;	// 3:rgb24
    rgb_line_pad = 0;
    rgb_line_pitch = width*rgb_type;
    while(rgb_line_pitch&0x3){
        rgb_line_pitch++;
        rgb_line_pad++;
    }
    pb = pRgb + (height-1)*rgb_line_pitch;
    rgb_line_pad = -rgb_line_pitch*2;
    if(rgb_type == 3){	// RGB24
        bs = 3;
    }
    else if(rgb_type == 2){	// RGB16 simple
        bs = -2;
    }
    else{
        return -1;
    }

    n_point = 0;
    for(i = 0;i < height;i++){
        for(j = 0;j < width;j++){
            y = *py;
            u = *pu;
            v = *pv;
#if 1
            // B...
            bt = 1192*(y - 16) + 2066*(u - 128);
            if(bt > 0)
                bt >>= 10;
            else
                bt = 0;
            // G
            gt = 1192*(y - 16) - 389*(u-128) - 833*(v - 128);
            if(gt > 0)
                gt >>= 10;
            else
                gt = 0;
            // R
            rt = 1192*(y - 16) + 1187*(v - 128);
            if(rt > 0)
                rt >>= 10;
            else
                rt = 0;
#elif 0
            // R...
            rt = y + 1.14*v;
            // G
            gt = y + 0.39*u - 0.58*v;
            // B
            bt = y + 2.03*v;
#elif 0
            rt = 1.164*(y - 16) + 1.159*(v-128);
            gt = 1.164*(y - 16) - 0.38*(u-128) - 0.813*(v-128);
            bt = 1.164*(y-16) + 2.018*(v-128);
            if(rt < 0) rt = 0;
            if(gt < 0) gt = 0;
            if(bt < 0) bt = 0;
#elif	1
            rt = static_cast<byte>(1.0*y + 8 + 1.402*(v-128));
            gt = static_cast<byte>(1.0*y - 0.34413*(u-128) - 0.71414*(v-128));
            bt = static_cast<byte>(1.0*y + 1.772*(u-128) + 0);
#endif

            if(rgb_type == 3){
                *pb = bt;
                *(pb+1) = gt;
                *(pb+2) = rt;
            }
            else if(rgb_type == 2){
                bt = ((bt>>3)&0x1F) | ((gt<<3)&0x7E0) | ((rt<<8)&0xF800);
                *pb = bt&0xFF;
                *(pb+1) = (bt>>8)&0xFF;
            }

            py += ys;
            n_point++;
            if(n_point%(1<<yuv_type) == 0){
                pu += us;
                pv += vs;
            }
            pb += bs;
        }
        if(yuv_left){
            py += yuv_left*ys;
            pu += (yuv_left >> yuv_type)*us;
            pv += (yuv_left >> yuv_type)*vs;
        }
        if(rgb_line_pad)
            pb += rgb_line_pad;
    }

    return 1;
}

int myRGBToYUV(unsigned char *pRgb,unsigned char *pYuv,
               int width,int height,int rgbType,int yuvType){
    int rgb_type;
    int yuv_type;
    int rgb_line_pitch;

    unsigned char *pb;
    int bs;
    unsigned char *py, *pu, *pv;
    unsigned int n;

    int i, j, n_point;
    unsigned char r, g, b;
    int rt, gt, bt;
    int x;

    if(rgbType != 3)	// 暂不支持
        return -1;
    rgb_type = rgbType; // 4: 32, 3: 24, 2: 16
    pb = pRgb;
    bs = 3;
    rgb_line_pitch = width*rgb_type;
    while (rgb_line_pitch &0x3)
        rgb_line_pitch++;

    if(yuvType != 1)	// 暂不支持
        return -1;
    yuv_type = yuvType; // 1: 422, 2: 420
    x = width*height;
    py = pYuv;
    pu = py + x;
    pv = pu + x/(1<<yuv_type);

    n_point = 0;
    for (i = height - 1; i >= 0 ; i--) {
        pb = pRgb + i*rgb_line_pitch;
        for (j = 0; j < width; j++) {
            if (rgb_type == 3) {
                b = *pb;
                g = *(pb + 1);
                r = *(pb + 2);
            } else if (rgb_type == 2) {
                unsigned int ii;

                ii = (*pb << 8) + (*(pb + 1));

                b = (ii >> 11);
                g = (ii >> 5) & 0x3F;
                r = ii & 0x1F;

                b <<= 3;
                g <<= 2;
                r <<= 3;
            } else
                return -1;

            pb += bs;

            // for Y
            rt = r*306;
            gt = g*601;
            bt = b*147;
            x = (rt+gt+bt)>>10;
            // x+-
            if (x > 235)
                x = 235;
            else if (x < 16)
                x = 16;
            *py++ = x;

            n_point++;
            if (n_point %(1<<yuv_type) == 0) {
                // for U
                rt = r*173;
                gt = g*339;
                bt = b*512;
                x = (bt-rt-gt );
                if (x > 0)
                    x = x>>10;
                else
                    x /= 1024;
                x += 128;
                // x+-
                if (x > 235)
                    x = 235;
                else if (x < 16)
                    x = 16;
                *pu++ = x;

                // for V
                rt = r*512;
                gt = g*429;
                bt = b*83;
                x = (rt-gt-bt);
                if (x > 0)
                    x = x>>10;
                else
                    x /= 1024;
                x += 128;
                // x+-
                if (x > 235)
                    x = 235;
                else if (x < 16)
                    x = 16;
                *pv++ = x;
            }
        }
    }

    return 1;
}

//
// 暂额外支持16进制,不支持运算
// todo: 支持算术运算,十进制十六进制混合,空格括号等
//
int myGetIntFromStr(const char *str, int *pVal) {
    int n;
    int len;
    int val;
    const char *pc;

    if (str == NULL || pVal == NULL)
        return -1;

    pc = str;
    while (*pc == ' ') pc++;
    if (*pc == '0' && (*(pc + 1) == 'x' || *(pc + 1) == 'X')) {
        pc += 2;
        n = sscanf(pc, "%x", pVal);
        if (n != 1)
            return -1;
        else
            return 1;
    }
    else {
        n = sscanf(pc, "%d", pVal);
        if (n == 1)
            return 1;
        else
            return -1;
    }
}

//
// 整数范围值,c为间隔字符(如'-'),返回识别的数字数目0-2
// 0-3返回0,3; 1返回1,1; 3-2返回2,3; 0-返回0,0; -3错误
// 0-3,0 - 3,0-
//
//
int myGetRangeUint(const char *s,char c,int *pmin,int *pmax,int sort){
#if 0
    const char *p;
    int len;
    int hit;

    if(s == NULL || pmin == NULL || pmax == NULL)
        return -1;

    len = strlen(s);
    p = s;
    while(len > 0){
        if(isdigit(*p))
            break;
        if(!isblank(*p))
            return -1;
        p++;
        len--;
    }
    if(len <= 0)
        return -1;


    if(c == 0)
        c = '-';

    len = sscanf(p,"%d",pmin);
    if(len != 1)
        return 0;

    // search next int
    hit = 0;
    len = strlen(p);
    while(len > 0){
        // some overlay works!!!
        if(hit && isdigit(*p)){
            break;
        }
        if(isdigit(*p) || isblank(*p)){
            p++;
            len--;
            continue;
        }
        if( *p != c ){
            *pmax = *pmin;
            return 1;
        }
        hit = 1;
        p++;
        len--;
    }
    if(len <= 0){
        *pmax = *pmin;
        return 1;
    }

    len = sscanf(p,"%d",pmax);
    if(len != 1){
        *pmax = *pmin;
        return 1;
    }

    if(sort){
        if(*pmax < *pmin){
            len = *pmax;
            *pmax = *pmin;
            *pmin = len;
        }
    }
#endif
    return 2;
}
//::WritePrivateProfileString
