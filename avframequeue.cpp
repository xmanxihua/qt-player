#include "avframequeue.h"
#include <QMutexLocker>
AVFrameQueue::AVFrameQueue(int maxSize) : maxSize(maxSize){
}


void AVFrameQueue::push_back(AVFrame* frame){

    QMutexLocker locker(&mutex);
    while (frames.size() >= maxSize) {
        //printf("AVFrameQueue full, maxSize=%d\n", maxSize);
        fullCondition.wait(&mutex);
    }

    frames.push_back(frame);
    emptyCondition.wakeOne();
}

AVFrame* AVFrameQueue::pop_front(){
    QMutexLocker locker(&mutex);
    while(frames.empty()) {
        emptyCondition.wait(&mutex);
    }

    AVFrame* fr = frames.front();

    frames.pop_front();
    fullCondition.wakeOne();

    return fr;
}


