#ifndef XAUDIOPLAY_H
#define XAUDIOPLAY_H
#include "noncopyable.h"
#include <QAudio>
#include <QObject>
#include <QAudioOutput>
#include "xiodevice.h"

class XAudioPlay : public QObject{
    Q_OBJECT
public:
    static XAudioPlay* getInstance();
    virtual int getFree();
    virtual void stop();
    virtual bool start();
    virtual void play(bool isPlay);
    virtual bool write(char* out,int pts ,int dataSize);



    QAudioOutput* output = NULL;
    XIODevice *io;

    int sampleRate = 48000;
    int sampleSize = 16;
    int channel = 2;


private slots:
      void handleStateChanged(QAudio::State state);
public:
    XAudioPlay();
};

#endif // XAUDIOPLAY_H
