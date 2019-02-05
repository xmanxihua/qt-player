#include "xffmpeg.h"
#include <iostream>
#include <string>
#include "utils.h"


XFFmpeg* XFFmpeg::getInstance() {
   static XFFmpeg xffmpeg;
   return &xffmpeg;
}

bool XFFmpeg::open(std::string datasource) {
    close();

    this->dataSource = datasource;
    this->ret = avformat_open_input(&this->ic, datasource.c_str(), NULL, NULL);
    if (ret !=0) {
        Utils::av_perr("avformat_open_input", ret);
        return false;
    }

    this->totalMs = (ic->duration / AV_TIME_BASE) * 1000;

    for (int i=0; i<ic->nb_streams; ++i) {
        AVCodecContext* ctx = ic->streams[i]->codec;
        if (ctx->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoAvcodecContext = ctx;
            videoStream = i;
        }
        else if (ctx->codec_type == AVMEDIA_TYPE_AUDIO) {
            audioStream  = i;
            audioAvcodecContext = ctx;
        }
    }

    if (videoAvcodecContext) {
        fps= Utils::r2d(ic->streams[videoStream]->avg_frame_rate);
        AVCodec* codec = avcodec_find_decoder(videoAvcodecContext->codec_id);
        if (codec == NULL) {
            return false;
        }
        videoAvcodecContext = avcodec_alloc_context3(codec);
        avcodec_parameters_to_context(videoAvcodecContext, ic->streams[videoStream]->codecpar);
        ret = avcodec_open2(videoAvcodecContext, codec, NULL);

        if (ret !=0) {
            return false;
        }

    }

    if (audioAvcodecContext) {
        AVCodec* codec = avcodec_find_decoder(audioAvcodecContext->codec_id);
        if (codec == NULL) {
            return false;
        }
        audioAvcodecContext = avcodec_alloc_context3(codec);
        avcodec_parameters_to_context(audioAvcodecContext, ic->streams[audioStream]->codecpar);
        ret = avcodec_open2(audioAvcodecContext, codec, NULL);

        if (ret !=0) {
            return false;
        }


        this->sampleRate = audioAvcodecContext->sample_rate;

        this->channels = audioAvcodecContext->channels;

        switch (audioAvcodecContext->sample_fmt) {
        case AV_SAMPLE_FMT_S16:
            this->sampleSize = 16;
            break;
        case AV_SAMPLE_FMT_S32:
            this->sampleSize = 32;
        case AV_SAMPLE_FMT_S16P:
            this->sampleSize = 16;
        case AV_SAMPLE_FMT_FLTP:
            this->sampleSize = 32;
            //this->sampeType = QAudioFormat::Float;
        default:
            break;
        }

        printf("sample size:%d, sample rate:%d, channel:%d\n", sampleSize, sampleRate, channels);
    }

    return true;
}

bool XFFmpeg::toRGB(AVFrame* yuv, char** out, int outwidth, int outheight) {
    if (!ic || !yuv) {
        return false;
    }

    *out=(char *)av_malloc(
                av_image_get_buffer_size(AV_PIX_FMT_BGRA,  outwidth, outheight ,1));

    swsctx = sws_getCachedContext(swsctx
                         , videoAvcodecContext->width
                         , videoAvcodecContext->height
                         , videoAvcodecContext->pix_fmt
                         , outwidth
                         , outheight
                         , AV_PIX_FMT_BGRA
                         , SWS_BICUBIC
                         , NULL, NULL, NULL);
    if (!swsctx) {
        return false;
    }

    uint8_t* data[AV_NUM_DATA_POINTERS] = {0};
    data[0] = (uint8_t*)(*out);
    int lineSize[AV_NUM_DATA_POINTERS] = {0};
    lineSize[0] = outwidth * 4;

    int h = sws_scale(swsctx
                      , yuv->data
                      , yuv->linesize
                      , 0
                      , videoAvcodecContext->height
                      , data
                      , lineSize);

    return true;
}

bool XFFmpeg::toPCM(AVFrame* pcm, int* pOutSize, char** out) {
    int dst_rate = 48000;
    int dst_nb_samples = 0;
    static int max_dst_nb_samples = 0;
    int dst_nb_channels = 2;
    int dst_linesize;
    enum AVSampleFormat dst_sample_fmt = AV_SAMPLE_FMT_S16;
    uint8_t **dst_data = NULL;
    int64_t dst_ch_layout;

    dst_ch_layout = av_get_default_channel_layout(dst_nb_channels);

    if (!ic || !pcm || !out){
        return false;
    }

    if (swrctx == NULL) {
        swrctx = swr_alloc();
        swr_alloc_set_opts(swrctx
                           , dst_ch_layout
                           , dst_sample_fmt
                           , dst_rate
                           , pcm->channel_layout
                           , audioAvcodecContext->sample_fmt
                           , pcm->sample_rate
                           ,0,NULL);

        swr_init(swrctx);

    }

    dst_nb_samples = av_rescale_rnd(swr_get_delay(swrctx, pcm->sample_rate) +
                                            pcm->nb_samples, dst_rate, pcm->sample_rate, AV_ROUND_UP);


    av_samples_alloc_array_and_samples(&dst_data, &dst_linesize, dst_nb_channels,
                                       dst_nb_samples, dst_sample_fmt, 0);
    if (dst_nb_samples > max_dst_nb_samples) {

        av_freep(&dst_data[0]);
        ret = av_samples_alloc(dst_data, &dst_linesize, dst_nb_channels,
                               dst_nb_samples, dst_sample_fmt, 1);
        if (ret < 0)
            return false ;
        max_dst_nb_samples = dst_nb_samples;
    }
    int len= swr_convert(swrctx
                         , dst_data
                         , dst_nb_samples
                         , (const uint8_t**)pcm->data
                         , pcm->nb_samples);

    if (len <=0){
        return 0;
    }

    int outSize = av_samples_get_buffer_size(&dst_linesize
                                             , dst_nb_channels
                                             , len
                                             , dst_sample_fmt
                                             , 1);

    *pOutSize = outSize;
    //char* tempOut = (char *)av_malloc(outSize);
    //memcpy(tempOut, (char*)dst_data[0], outSize);
    //*out = tempOut;
    //av_free((char*)dst_data[0]);
    *out = (char*)dst_data[0];

    return true;
}

void XFFmpeg::close() {
    if (ic) {
        avformat_close_input(&ic);
    }

    if (swsctx) {
        sws_freeContext(swsctx);
        swsctx = NULL;
    }

    if (swrctx) {
        swr_free(&swrctx);
    }



    this->totalMs = 0;
    this->dataSource = "";
}

bool XFFmpeg::seek(double pos) {
    int pts = pos* ic->streams[videoStream]->duration;
    int ret = av_seek_frame(ic, videoStream, pts, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
    if (ret ==0) {
        avcodec_flush_buffers(videoAvcodecContext);
    }

    return ret >= 0;
}

AVPacket XFFmpeg::read() {
    AVPacket avPacket={0};
    if (!ic) {
        return avPacket;
    }
    int ret = av_read_frame(ic, &avPacket);
    if (ret !=0) {
        Utils::av_perr("av_read_frame error", ret);
    }
    return avPacket;
}

void XFFmpeg::decode(AVPacket* pkt, AVFrameQueue* queue) {
    if (pkt->size == 0) {
        return;
    }

    AVCodecContext *ctx = videoAvcodecContext;
    if (pkt->stream_index == audioStream) {
         ctx=audioAvcodecContext;
    }
    int ret = avcodec_send_packet(ctx, pkt);
    if (ret < 0) {
        if (ret != AVERROR(EAGAIN) && ret !=AVERROR_EOF){

            if (ret == AVERROR(EINVAL)){
                fprintf(stderr, "AVERROR(EINVAL) Error during decoding\n");
            }

            if (ret == AVERROR(ENOMEM)) {
                fprintf(stderr, "AVERROR(ENOMEM) Error during decoding\n");
            }

            fprintf(stderr, "Error during decoding\n");
            //exit(1);
        }

    }


        AVFrame* frame= av_frame_alloc();
        ret = avcodec_receive_frame(ctx, frame);

        if ( ret == AVERROR_EOF){
            return;
        }else if (ret == AVERROR(EAGAIN)){
            return;
        }
        else if (ret < 0 && ret != AVERROR_EOF && ret != AVERROR(EAGAIN)) {
            fprintf(stderr, "Error during decoding\n");
            exit(1);
        }
        queue->push_back(frame);
        int p = frame->pts * Utils::r2d(ctx->time_base) * 1000;
        if (pkt->stream_index == audioStream){
            pts = p;
        }




}

void XFFmpeg::printError(std::string usage) {
    Utils::av_perr(usage, ret);
}

XFFmpeg::XFFmpeg () {
    av_register_all();
}

XFFmpeg::~XFFmpeg() {
}
