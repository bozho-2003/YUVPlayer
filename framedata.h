#ifndef FRAMEDATA_H
#define FRAMEDATA_H
#include "common.h"
#include <QByteArray>

class FrameData
{
public:
    FrameData();
    void setWidth(int i)
    {
        m_width = i;
    }

    void setHeight(int i)
    {
        m_height = i;
    }
    int getWidth()
    {
        return m_width;
    }

    int getHeight()
    {
        return m_height;
    }

    void setType(FrameType i)
    {
        m_type = i;
    }

    FrameType getType()
    {
        return m_type;
    }

    void mallocBuffer();
    QByteArray *getBuffer();
private:
    int m_width;
    int m_height;
    FrameType m_type;
    QByteArray m_framebuffer;

};

#endif // FRAMEDATA_H
