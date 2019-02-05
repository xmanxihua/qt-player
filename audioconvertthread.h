#ifndef AUDIOCONVERTTHREAD_H
#define AUDIOCONVERTTHREAD_H

#include <QThread>
#include "avframequeue.h"
class AudioConvertThread : public QThread
{
private:
    AudioConvertThread();
public:

    void run();
    virtual ~AudioConvertThread();
    AVFrameQueue queue;
    int pts;
    static AudioConvertThread* getInstance() {
        static AudioConvertThread t;
        return &t;
    }
};

#endif // AUDIOCONVERTTHREAD_H
