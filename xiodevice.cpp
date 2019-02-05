#include "xiodevice.h"
#include "xffmpeg.h"

XIODevice::XIODevice()
{
    this->open(QIODevice::ReadOnly);
}

qint64 XIODevice::readData(char *data, qint64 maxlen){ //重新实现的虚函数
    int _pts;
    int count = queue.pop_back(data, maxlen, &_pts);
    XFFmpeg::getInstance()->audio_pts = _pts;
    return count;
}

qint64 XIODevice::write(const char *data, int pts, qint64 len){ //它是个纯虚函数， 不得不
    queue.push_front(data, pts, len);
}

qint64 XIODevice::writeData(const char *data, qint64 len){ //它是个纯虚函数， 不得不
    printf("XIODevice::writeData\n");
    return 0;
}
