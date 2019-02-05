#ifndef XIODEVICE_H
#define XIODEVICE_H
#include <QAudioOutput>
#include <list>
#include <QMutex>
#include <QWaitCondition>

extern "C" {
#include <libavutil/mem.h>
}


struct PCMData {
    const char* data;
    int len;
    int pts;
    int index;
};

class PCMDataQueue {
    int maxLen;
    std::list<PCMData> datas;

    QMutex mutex;
    QWaitCondition emptyCondition;
    QWaitCondition fullCondition;
public:
    int pop_back(char* out, int len, int* ppts) {
        QMutexLocker locker(&mutex);
        printf("PCMDataQueue:len=%d\n", len);
        while (datas.empty()) {
            //printf("PCMDataQueue full, maxSize=%d\n ", 5);
            emptyCondition.wait(&mutex);
        }
        if (maxLen<len) maxLen=len;

        int pts = 0;
        int popCount = 0;
        while(!datas.empty()) {
            PCMData pcmData = datas.back();
            if (pts == 0) {
                pts = pcmData.pts;
            }
            if(len <= 0) {
                break;
            }
            int realLen = len < pcmData.len - pcmData.index ? len : pcmData.len - pcmData.index;
            memcpy(out, pcmData.data, realLen);

            len -= realLen;
            out += realLen;
            popCount += realLen;

            pcmData.index += realLen;
            if (pcmData.len == pcmData.index){
                datas.pop_back();
                av_free((void*)pcmData.data);
                continue;
            }
        };
        //fullCondition.wakeAll();
        *ppts = pts;
        return popCount;
    }

    void push_front(const char* data, int pts,  int len) {
        QMutexLocker locker(&mutex);
//        while (datas.size() > 1){
//            //printf("PCMDataQueue full, maxSize=%d\n ", 1);
//            fullCondition.wait(&mutex);
//        }
        PCMData pcmData;
        pcmData.data = data;
        pcmData.len = len;
        pcmData.pts = pts;
        pcmData.index = 0;
        datas.push_front(pcmData);


        emptyCondition.wakeAll();

    }

    bool empty() {
        QMutexLocker locker(&mutex);
        return datas.empty();
    }
};

class XIODevice : public QIODevice
{
public:
    XIODevice();
    qint64 readData(char *data, qint64 maxlen); //重新实现的虚函数
    qint64 writeData(const char *data, qint64 len); //它是个纯虚函数， 不得不实现
    qint64 write(const char *data, int pts, qint64 len);
private:
    PCMDataQueue queue;
};

#endif // XIODEVICE_H
