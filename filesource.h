#ifndef YUVREADER_H
#define YUVREADER_H

#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QString>
#include "common.h"
#include <QByteArray>
#include <stdio.h>
//#include "framedata.h"


#define FILE_INVALID -1
class FileSource
{
public:
    //explicit FileSource(QObject *parent = 0);
    explicit FileSource();
    ~FileSource();

    int init(QString fn, int skip_bytes);

    int setBlockSize(int block_size);
    int getBlockNum();


    int seekForward(int block_num);

    int seekBackward(int block_num);

    int restart();
    int getBlockIndex();
    int fillData(unsigned char *buf, int *p_bufsz);

    int destroy();
private:
    QFile m_file;
    qint64 m_block_size;
    qint64 m_header_skip;
    qint64 m_file_size;
    bool m_readyFlag;
};

#endif // YUVREADER_H
