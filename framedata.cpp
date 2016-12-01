#include "framedata.h"
#include <stdlib.h>

FrameData::FrameData()
{

}

void FrameData::mallocBuffer()
{
    m_framebuffer = NULL;
    switch (m_type)
    {
    case YUV420:
        //m_framebuffer = (char *)malloc(m_width * m_height * 3 /2);
        break;
    }
}

QByteArray *FrameData::getBuffer()
{
    return &m_framebuffer;™
}


