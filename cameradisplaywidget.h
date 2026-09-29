#ifndef CAMERADISPLAYWIDGET_H
#define CAMERADISPLAYWIDGET_H

#include <QWidget>
#include <QImage>

class CameraDisplayWidget : public QWidget
{
public:
    explicit CameraDisplayWidget(QWidget *parent = nullptr);

    void setImage(const QImage &image);
    void clearImage();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QImage currentImage;
};

#endif