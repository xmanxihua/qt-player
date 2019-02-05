#include "xplayopenglwidget.h"
#include "xffmpeg.h"

#include <QPainter>
#include <QDateTime>
#include "videoconvertthread.h"
#include "audioconvertthread.h"

XPlayOpenGLWidget::XPlayOpenGLWidget(QWidget* widget):QOpenGLWidget(widget){

}

XPlayOpenGLWidget::~XPlayOpenGLWidget(){

}


void XPlayOpenGLWidget::paintEvent(QPaintEvent *e){

    qint64 timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();

    QPainter painter;

    static QImage* image = NULL;
    static int h = 0;
    static int w = 0;
    if (h!=height() || w!=width()) {
        if (image != NULL) {
            delete image->bits();
            delete image;
            image = NULL;
        }
    }

    if (image == NULL){

        uchar* buf = new uchar[width()*height()*4];
        image = new QImage(buf, width(), height(), QImage::Format_ARGB32 );
    }

    qint64 rgbBegin = QDateTime::currentDateTime().toMSecsSinceEpoch();

    YUVData out = VideoConvertThread::getInstance()->yuvDataQueue.pop_back();
    killTimer(timerId);
    if (XFFmpeg::getInstance()->audio_pts < out.pts){
        printf("out.pts=%d, audio.pts=%d\n", out.pts, XFFmpeg::getInstance()->audio_pts);
        //QThread::msleep(out.pts - XFFmpeg::getInstance()->audio_pts);

        timerId= startTimer(out.pts-XFFmpeg::getInstance()->audio_pts);
    }
    else {
        timerId = startTimer(10);
    }
    memcpy(image->bits(), out.data, width()*height()*4);
    //XFFmpeg::getInstance()->toRGB(frame, (char*)image->bits(), width(), height());
    //printf("rgb time=%lld\n", QDateTime::currentDateTime().toMSecsSinceEpoch() - rgbBegin);
    painter.begin(this);
    painter.drawImage(QPoint{ 0,0,}, *image );
    painter.end();
    av_free(out.data);
    //printf("time=%lld\n", QDateTime::currentDateTime().toMSecsSinceEpoch() - timestamp);
}

void XPlayOpenGLWidget::timerEvent(QTimerEvent* e){

    this->update();
}
