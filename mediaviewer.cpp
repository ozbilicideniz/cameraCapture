#include "mediaviewer.h"
#include <QGridLayout>
#include <QDir>
#include <QPixmap>
#include <QLabel>
#include <QPalette>
#include <QColor>

MediaViewer::MediaViewer(QWidget *parent)
    : QWidget(parent)
{
    setAutoFillBackground(true);
    setAttribute(Qt::WA_StyledBackground, true);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(35, 35, 35));
    setPalette(pal);

    mediaGrid = new QGridLayout(this);
    QDir photoDir("/root/camera/photos");
    QStringList photos = photoDir.entryList(QStringList() << "*.jpg" << "*.jpeg", QDir::Files);


    int index = 0;

    for (const QString &photoName : photos) {
        QString fullPath = photoDir.filePath(photoName);
        QPixmap pixmap(fullPath);
        QLabel *thumbnail = new QLabel();
        QPixmap scaledPixmap = pixmap.scaled(250, 150, Qt::KeepAspectRatio);
        thumbnail->setPixmap(scaledPixmap);

        int row = index / 4;
        int column = index % 4;
        mediaGrid->addWidget(thumbnail, row, column);
        index++;

    }

}
