#ifndef XPLAY_H
#define XPLAY_H

#include <QWidget>

namespace Ui {
class XPlay;
}

class XPlay : public QWidget
{
    Q_OBJECT

public:
    explicit XPlay(QWidget *parent = 0);
    ~XPlay();
    void timerEvent(QTimerEvent *event);

private slots:

    void on_openButton_clicked();


    void openFile(QString);

    void on_playProgress_sliderPressed();

    void on_playProgress_sliderReleased();

    void on_playButton_clicked();

    void resizeEvent(QResizeEvent * event);

private:
    Ui::XPlay *ui;
    bool isPlay;
    bool isOpened;
};

#endif // XPLAY_H
