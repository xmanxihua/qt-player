#ifndef XPLAYOPENGLWIDGET_H
#define XPLAYOPENGLWIDGET_H
#include <list>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
}

#include "xffmpeg.h"

#include <QOpenGLWidget>
#include <QMutex>
#include <QDateTime>
#include <QThread>
#include <QWaitCondition>
#include "utils.h"



class XPlayOpenGLWidget : public QOpenGLWidget
{
public:
    XPlayOpenGLWidget(QWidget* widget);
    virtual ~XPlayOpenGLWidget();
    void paintEvent(QPaintEvent *e);
    int timerId;


    void timerEvent(QTimerEvent* e);
};

#endif // XPLAYOPENGLWIDGET_H
