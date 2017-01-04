#include "filesource.h"

FileSource::FileSource()//(QObject *parent) : QObject(parent)
{
    m_block_size = 1;
    m_header_skip = 0;
    m_file_size = 0;
    m_readyFlag  = false;
}


FileSource::~FileSource(){
    destroy();
}

int FileSource::init(QString fn, int skip_bytes){
    destroy();

    if(fn == NULL)
        return -1;

    m_file.setFileName(fn);

    if (!m_file.open(QIODevice::ReadOnly)) {
        return -1;
    }

    QFileInfo info(fn);
    m_file_size = info.size();

    if(skip_bytes < 0)
        skip_bytes = 0;
    m_header_skip = skip_bytes;
    m_file.seek(m_header_skip);
    return 1;
}

int FileSource::setBlockSize(int block_size){
    if(block_size <= 0)
        return -1;
    m_block_size = block_size;
    return 1;
}
int FileSource::getBlockNum(){
    return (int)((m_file_size - m_header_skip)/(qint64)m_block_size);
}


int FileSource::seekForward(int block_num){
    qint64 offset = block_num * m_block_size;
    qint64 pos = m_file.pos();
    if(offset + pos + m_header_skip > m_file_size - m_block_size){
        qint64 fsz = m_file.size();

        if(offset + pos + m_header_skip > fsz - m_block_size)
            return 0;  // file EOF
        m_file_size = fsz;
    }

    m_file.seek(pos + offset);
    qint64 pos0 = m_file.pos();
    return 1;
}

int FileSource::seekBackward(int block_num){
    qint64 offset = block_num*m_block_size;
    qint64 pos = m_file.pos();
    if(offset > pos - m_header_skip)
        return 0;  // file EOF
    m_file.seek(pos - offset);
    return 1;
}

int FileSource::restart(){
    qint64 pos = m_file.pos();
    pos = -pos;
    return m_file.seek(m_header_skip);
}

int FileSource::getBlockIndex(){
    qint64 pos = m_file.pos();
        return (pos - m_header_skip)/m_block_size;
}

int FileSource::fillData(unsigned char *buf, int *p_bufsz){
    if(buf == NULL || p_bufsz == NULL || *p_bufsz <= 0)
        return -1;
    *p_bufsz = m_file.read((char *)buf, *p_bufsz);
    qint64 pos = m_file.pos();
    if(*p_bufsz > 0){
        m_file.seek(pos-*p_bufsz);
        return 1;
    }
    return -2;
}

int FileSource::destroy(){
    /*
    if(m_file != MFILE_INVALID){
        mCloseFile(m_file);
        m_file = MFILE_INVALID;
    }
    */
    m_file.close();
    m_block_size = 1;
    m_header_skip = 0;
    m_file_size = 0;
    return 1;
}

