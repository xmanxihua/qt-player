#include "xaudioplay.h"
#include <QAudioOutput>
#include "xiodevice.h"

XAudioPlay::XAudioPlay():io(new XIODevice){

}


void XAudioPlay::stop() {
    if (output){
        output->stop();
        delete output;
        output = NULL;
        io = NULL;
    }
}


bool XAudioPlay::start() {
    stop();
    QAudioFormat fmt;
    fmt.setSampleRate(sampleRate);
    fmt.setSampleSize(sampleSize);
    fmt.setChannelCount(channel);
    fmt.setCodec("audio/pcm");
    fmt.setByteOrder(QAudioFormat::LittleEndian);
    fmt.setSampleType(QAudioFormat::SignedInt);

    output = new QAudioOutput(fmt);

    connect(output, SIGNAL(stateChanged(QAudio::State)), this, SLOT(handleStateChanged(QAudio::State)));
    output->start(io);

    return true;
}

void XAudioPlay::play(bool isPlay){
    if (!output) return;

    if (isPlay) {
        output->resume();
    }
    else {
        output->suspend();
    }
}

int XAudioPlay::getFree(){
    return output == NULL ? 0 : output->bytesFree();
}


bool XAudioPlay::write(char* out, int  pts, int datasize) {
    if (io){
        io->write(out, pts, datasize);
    }

    return true;
}

void XAudioPlay::handleStateChanged(QAudio::State state){
    printf("handleStateChanged:new state=%d\n", state);
}

XAudioPlay* XAudioPlay::getInstance(){
    static XAudioPlay p;
    return &p;
}
