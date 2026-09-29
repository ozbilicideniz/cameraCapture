#ifndef CAMERAWIDGET_H
#define CAMERAWIDGET_H

#include <QWidget>
#include <QImage>
#include <QTimer>

class cameraWidget : public QWidget
{
    Q_OBJECT

public:
    explicit cameraWidget(QWidget *parent = nullptr);
    ~cameraWidget();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    struct Buffer
    {
        void *start = nullptr;
        size_t length = 0;
    };

    int cameraFd = -1;
    Buffer *buffers = nullptr;
    unsigned int bufferCount = 0;

    QImage currentFrame;
    QTimer *frameTimer = nullptr;

    bool startCamera();
    void captureFrame();
    void stopCamera();
};

#endif


