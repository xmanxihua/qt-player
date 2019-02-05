#ifndef VIDEOTHREAD_H
#define VIDEOTHREAD_H

#include <QThread>
#include "noncopyable.h"
#include "audioconvertthread.h"
#include "videoconvertthread.h"

class DecodeThread : public QThread, public noncopyable
{
private:
    DecodeThread();
public:
    static DecodeThread* getInstance() {
        static DecodeThread decodeThread;
        return &decodeThread;
    }

    void run();
    virtual ~DecodeThread();
};

#endif // VIDEOTHREAD_H
