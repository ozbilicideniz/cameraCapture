#include "camerawidget.h"
#include <fcntl.h>
#include <unistd.h>
#include <QDebug>

cameraWidget::cameraWidget(QWidget *parent)
    : QWidget(parent)
{
    cameraFd = open("/dev/video9", O_RDWR | O_NONBLOCK);

    if (cameraFd < 0) {
        qDebug() << "Could not open camera";
    } else {
        qDebug() << "Camera opened successfully";
    }
}