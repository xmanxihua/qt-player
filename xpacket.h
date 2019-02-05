#ifndef XPACKET_H
#define XPACKET_H
#include "noncopyable.h"


extern "C" {
#include <libavformat/avformat.h>
}

class XPacket:public noncopyable
{
public:
    XPacket(AVPacket avPacket, bool readSuccess = false);
    ~XPacket();
    AVPacket avPacket;
    bool readSuccess;
};

#endif // XPACKET_H
