#include "audioconvertthread.h"
#include "xffmpeg.h"
#include "xaudioplay.h"
#include "utils.h"

AudioConvertThread::AudioConvertThread():queue(100)
{
}

void AudioConvertThread::run(){
    for (;;){
        int len = 0;
        char* out = NULL;
        AVFrame* frame = queue.pop_front();
        XFFmpeg::getInstance()->toPCM(frame, &len, &out);
        int pts = frame->pts* Utils::r2d(XFFmpeg::getInstance()->ic->streams[XFFmpeg::getInstance()->audioStream]->time_base)*1000;
        XAudioPlay::getInstance()->write(out,pts, len);

        av_frame_free(&frame);
    }
}

AudioConvertThread::~AudioConvertThread(){

}
