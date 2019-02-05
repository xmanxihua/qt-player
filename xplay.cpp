#include "xplay.h"
#include "ui_xplay.h"
#include <QFileDialog>
#include "xffmpeg.h"
#include <QMessageBox>
#include "decodethread.h"
#include "xaudioplay.h"
#include "audioconvertthread.h"
#include "videoconvertthread.h"

XPlay::XPlay(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::XPlay)
{
    ui->setupUi(this);
    openFile("1.mp4");
}

XPlay::~XPlay()
{
    delete ui;
}


void XPlay::openFile(QString name) {
    if (!XFFmpeg::getInstance()->open(name.toStdString())){
        QMessageBox::information(this, "open file", "open failed");
        return;
    }

    isOpened = true;
    VideoConvertThread::getInstance()->width = ui->openGLWidget->width();
    VideoConvertThread::getInstance()->height = ui->openGLWidget->height();
    DecodeThread::getInstance()->start();
    ui->openGLWidget->timerId =  ui->openGLWidget->startTimer(20);

    //XAudioPlay::getInstance()->sampleRate = XFFmpeg::getInstance()->sampleRate;
    //XAudioPlay::getInstance()->sampleSize =  XFFmpeg::getInstance()->sampleSize;
    //XAudioPlay::getInstance()->channel = XFFmpeg::getInstance()->channel;
    XAudioPlay::getInstance()->start();
    this->setWindowTitle(name);

    int min = XFFmpeg::getInstance()->totalMs /1000 /60;
    int sec = XFFmpeg::getInstance()->totalMs / 1000 % 60;
    char buf[1024] = {0};
    sprintf(buf, "%d:%d", min, sec);
    ui->totalSecs->setText(buf);
    on_playButton_clicked();

    startTimer(10);
}

void XPlay::on_openButton_clicked()
{
    QString name= QFileDialog::getOpenFileName(this, "choose file");
    if (name.isEmpty()) {
        return;
    }


    openFile(name);

}
static bool isPressed =false;

void XPlay::timerEvent(QTimerEvent *event) {
    int min = XFFmpeg::getInstance()->pts /1000 /60;
    int sec = XFFmpeg::getInstance()->pts / 1000 % 60;
    char buf[1024] = {0};
    sprintf(buf, "%d:%d", min, sec);
    ui->playTime->setText(buf);
    if (!isPressed) {
        double rate = (double)XFFmpeg::getInstance()->pts / (double)XFFmpeg::getInstance()->totalMs;
        ui->playProgress->setValue(rate * 1000);
    }
}

void XPlay::on_playProgress_sliderPressed()
{
    isPressed = true;
}

void XPlay::on_playProgress_sliderReleased()
{
    XFFmpeg::getInstance()->seek((double)ui->playProgress->value()/(double)(ui->playProgress->maximum()+1));
    isPressed = false;
}

void XPlay::on_playButton_clicked()
{
    if (!isOpened){
        return;
    }
    isPlay = !isPlay;

    XFFmpeg::getInstance()->isPlay = isPlay;


    ui->playButton->setText(isPlay ? "pause" : "play");
}

void XPlay::resizeEvent(QResizeEvent * event){
    ui->openGLWidget->resize(size());
}
