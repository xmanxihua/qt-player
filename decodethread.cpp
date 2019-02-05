#include "decodethread.h"
#include "xffmpeg.h"
#include <QDateTime>
#include "audioconvertthread.h"

DecodeThread::DecodeThread()
{
    VideoConvertThread::getInstance()->start();
    AudioConvertThread::getInstance()->start();
}


DecodeThread::~DecodeThread(){


}
bool isExit = false;
void DecodeThread::run() {
    char* out = NULL;
    while (!isExit) {

        if (!XFFmpeg::getInstance()->isPlay){
            msleep(10);
            continue;
        }

        qint64 timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
        AVPacket pkt = XFFmpeg::getInstance()->read();
        if (pkt.size == 0) {
            printf("pkt.size == 0");
            msleep(10);
            continue;
        }

        if (pkt.stream_index == XFFmpeg::getInstance()->videoStream) {
            XFFmpeg::getInstance()->decode(&pkt,&VideoConvertThread::getInstance()->frameQueue);

        }else if (pkt.stream_index == XFFmpeg::getInstance()->audioStream){

            XFFmpeg::getInstance()->decode(&pkt, &AudioConvertThread::getInstance()->queue);
        }
        av_packet_unref(&pkt);
    }
}
