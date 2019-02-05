#ifndef AVFRAMEQUEUE_H
#define AVFRAMEQUEUE_H

#include <list>
#include <QMutex>
#include <QWaitCondition>

extern "C" {
#include <libavformat/avformat.h>
}

class AVFrameQueue {
public:
    AVFrameQueue(int maxSize);
    void push_back(AVFrame* frame);
    AVFrame* pop_front();
private:
    QMutex mutex;
    QWaitCondition fullCondition;
    QWaitCondition emptyCondition;
    std::list<AVFrame*> frames;
    int maxSize;
};

#endif // AVFRAMEQUEUE_H
