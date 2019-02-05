#ifndef XFFMPEG_H
#define XFFMPEG_H
#include "noncopyable.h"
#include <string>
#include <QMutex>
#include "avframequeue.h"
extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libswresample/swresample.h>
#include <libavutil/imgutils.h>
}
class XFFmpeg : public noncopyable
{
public:
    bool open(std::string datasource);
    void close();
    void printError(std::string usage);
    AVPacket read();
    bool seek(double pos);
    void decode(AVPacket* pkt, AVFrameQueue* queue);
    bool toRGB(AVFrame* yuv, char** out, int outwidth, int outheight);
    bool toPCM(AVFrame* pcm, int* pOutSize, char** out);

    virtual ~XFFmpeg();

    static XFFmpeg* getInstance();
    int videoStream;
    int audioStream;
    int fps;
    long long totalMs;
    int pts;
    bool isPlay;

    int sampleRate = 48000;
    int sampleSize = 16;
    int channels = 2;
    AVCodecContext* videoAvcodecContext;
    //QAudioFormat::SignedInt/QAudioFormat::UnSignedInt/QAudioFormat::Float
    //int sampeType = QAudioFormat::UnSignedInt;
    std::string dataSource;
    XFFmpeg();
    AVFormatContext* ic;
    QMutex mutex;
    int ret;

    int audio_pts;

    AVCodecContext* audioAvcodecContext;
    SwsContext* swsctx;
    SwrContext* swrctx;


};

#endif // XFFMPEG_H
