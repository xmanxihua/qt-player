#include "videoconvertthread.h"
#include "xffmpeg.h"
#include "utils.h"

VideoConvertThread::VideoConvertThread():frameQueue(1000)
{
}
void VideoConvertThread::run() {

    for (;;){
        AVFrame * frame = frameQueue.pop_front();

        char* out = NULL;

        bool ret = XFFmpeg::getInstance()->toRGB(frame, &out, width, height);

        int pts = frame->pts * Utils::r2d(XFFmpeg::getInstance()->ic->streams[XFFmpeg::getInstance()->videoStream]->time_base)*1000;
        av_frame_free(&frame);
        if (ret) {
            YUVData data;
            data.data = out;
            data.pts = pts;
            yuvDataQueue.push_front(data);
        }
    }

}

