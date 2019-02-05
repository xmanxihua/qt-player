#include "xpacket.h"

extern "C" {
#include <libavformat/avformat.h>
}

XPacket::XPacket(AVPacket avPacket, bool readSuccess) : avPacket(avPacket), readSuccess(readSuccess)
{
}

XPacket::~XPacket() {
    if (readSuccess) {
        av_packet_unref(&avPacket);
    }
}
