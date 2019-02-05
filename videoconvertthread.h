#ifndef VIDEOCONVERTTHREAD_H
#define VIDEOCONVERTTHREAD_H
#include <QThread>
#include "avframequeue.h"

struct YUVData {
    char* data;
    int pts;
};

class YUVDataQueue {
    std::list<YUVData> datas;
    QMutex mutex;
    QWaitCondition fullCondition;
    QWaitCondition emptyCondition;
public:
    YUVData pop_back() {
        QMutexLocker locker(&mutex);
        while (datas.empty()) {
            //printf("YUVDataQueue full, maxSize=%d\n ", 5);
            emptyCondition.wait(&mutex);
        }

        YUVData yuv = datas.back();
        datas.pop_back();
        fullCondition.wakeAll();
        return yuv;
    }

    void push_front(YUVData yuv) {
        QMutexLocker locker(&mutex);
        while (datas.size() > 5){
            fullCondition.wait(&mutex);
        }
        datas.push_front(yuv);
        emptyCondition.wakeAll();
    }

    bool empty() {
        QMutexLocker locker(&mutex);
        return datas.empty();
    }
};

class VideoConvertThread : public QThread
{
public:
    int width; int height;
    AVFrameQueue frameQueue;
    YUVDataQueue yuvDataQueue;
    VideoConvertThread();
    void run();
    static VideoConvertThread* getInstance(){
        static VideoConvertThread t;
        return &t;
    }
};

#endif // VIDEOCONVERTTHREAD_H
