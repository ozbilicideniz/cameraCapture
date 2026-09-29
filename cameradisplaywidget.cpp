#include "cameradisplaywidget.h"

#include <QPainter>

CameraDisplayWidget::CameraDisplayWidget(QWidget *parent)
    : QWidget(parent)
{
}

void CameraDisplayWidget::setImage(const QImage &image)
{
    currentImage = image;
    update();
}

void CameraDisplayWidget::clearImage()
{
    currentImage = QImage();
    update();
}

void CameraDisplayWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    if (!currentImage.isNull()) {
        painter.drawImage(rect(), currentImage);
    }
}