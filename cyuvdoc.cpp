#include "cyuvdoc.h"
#include "common.h"

void CYUVDoc::_init(){
    m_pFormater = NULL;
    m_formaterId = -1;

    m_bLoop = 0;
}

CYUVDoc::CYUVDoc(){
    _init();
}

CYUVDoc::~CYUVDoc(){
    destroy();
}


int CYUVDoc::getFormaterID(){
    if(m_pFormater == NULL)
        return -1;
    return m_formaterId;
}
int CYUVDoc::setFormater(IYUV_Formater *formater, int id){
    if(formater == NULL)
        return -1;
    /*
    if(m_pFormater){
        m_pFormater->destroy();
    }
    */
    m_pFormater = formater;
    m_formaterId = id;
    return 1;
}

int CYUVDoc::setParams(YUV_Params *pParams){
    if(pParams == NULL)
        return -1;
    m_params = *pParams;
    return m_pFormater->setParams(pParams);
}

int CYUVDoc::getOutFormat(RawImage_Info *pInfo){
    return m_pFormater->getOutInfo(pInfo);
}

int CYUVDoc::setFileName(const char *fileName){
    if(fileName == NULL)
        return -1;
    m_fileName = fileName;
    return 1;
}

int CYUVDoc::update(void* pErr){
    if(m_pFormater == NULL)
        return -1;

    if(m_pFormater->setParams(&m_params) < 0){
        m_errMsg = "unsupportted yuv params";
        return -1;
    }
    if(m_pFormater->update(NULL) < 0){
        m_errMsg = QString("unsupportted yuv params %1").arg("aaa");
        return -1;
    }
    if(m_fileSource.init(QString(m_fileName), 0) < 0){
        m_errMsg = QString("open file \"%1\" failed.").arg(m_fileName);
        return -1;
    }

    int block_size = m_pFormater->getMinBlockSize();
    if(block_size <= 0){
        m_errMsg = QString("invalid block size: %1").arg(block_size);
        return -2;
    }
    m_fileSource.setBlockSize(block_size);
    if(m_fileSource.getBlockNum() < 1){
        m_errMsg = QString("small file");
        return -3;
    }

    if(updateFrame() < 0)
        return -4;

    return 1;
}

int CYUVDoc::getFrame(unsigned char *pBuf, int bufSz){
    if(m_pFormater->getFrame(0, pBuf, bufSz) < 0)
        return -2;
    return 1;
}

int CYUVDoc::updateFrame(){
    if(m_pFormater == NULL || m_fileSource.getBlockNum() <= 0)
        return -1;
    unsigned char *pBuf;
    int bufsz = m_pFormater->getMinBlockSize();
    m_pFormater->getBuffer(&pBuf);
    m_fileSource.fillData(pBuf, &bufsz);
    m_pFormater->putData(pBuf);
    return 1;
}

int CYUVDoc::seekFrameForward(int frame_num){
    if(frame_num < 0)
        return -1;
    int frame_index = m_pFormater->getFrameIndex();
    if(frame_index < 0)
        return -2;

    int block_frame_num = m_pFormater->getMinBlockFrameNum();
    int frame_left = block_frame_num - frame_index - 1;
    if(frame_num <= frame_left){
        m_pFormater->setFrameIndex(frame_index + frame_num);
        return 1;
    }

    frame_num -= frame_left;
    frame_index = (frame_num+1)%block_frame_num;
    int block_num = (frame_num + block_frame_num - 1)/block_frame_num;
    //if(frame_num%block_frame_num)
    //	block_num += 1;

    int ret = m_fileSource.seekForward(block_num);
    if(ret <= 0)
        return ret;

    if(updateFrame() < 0)
        return -3;
    if(frame_index > 0){
        m_pFormater->setFrameIndex(frame_index);
    }

    return 1;
}
int CYUVDoc::seekFrameBackward(int frame_num){
    if(frame_num < 0)
        return -1;
    int frame_index = m_pFormater->getFrameIndex();
    if(frame_index < 0)
        return -2;

    int block_frame_num = m_pFormater->getMinBlockFrameNum();
    int frame_left = frame_index;
    if(frame_num <= frame_left){
        m_pFormater->setFrameIndex(frame_left - frame_num);
        return 1;
    }

    frame_num -= frame_left;
    frame_index = frame_num%block_frame_num;
    int block_num = frame_num/block_frame_num;
    if(frame_index > 0)
        block_num++;
    int ret = m_fileSource.seekBackward(block_num);
    if(ret <= 0)
        return ret;

    if(updateFrame() < 0)
        return -3;
    if(frame_index > 0){
        frame_index = block_frame_num - frame_index;
        m_pFormater->setFrameIndex(frame_index);
    }
    return 1;
}

int CYUVDoc::restart(){
    m_fileSource.restart();
    updateFrame();
    return 1;
}
int CYUVDoc::seekFrame(int frame_num, int start){
    if(start == SEEK_CUR){
        if(frame_num >= 0)
            return seekFrameForward(frame_num);
        else
            return seekFrameBackward(frame_num);
    }
    else if(start == SEEK_SET){
        restart();
        seekFrameForward(frame_num);
    }
    return 1;
}
int CYUVDoc::getFrameNum(){
    return m_fileSource.getBlockNum()*m_pFormater->getMinBlockFrameNum();
}
int CYUVDoc::getFrameIndex(){
    int index;
    index = m_fileSource.getBlockIndex()*m_pFormater->getMinBlockFrameNum() + m_pFormater->getFrameIndex();
    return index + 1;
}

int CYUVDoc::destroy(){
    m_fileSource.destroy();
    /*
    if(m_pFormater)
        m_pFormater->destroy();
        */
    _init();
    return 1;
}

const QString CYUVDoc::getErrMsg(){
    if(m_errMsg.isEmpty())
        return "no error";
    return m_errMsg;
}
