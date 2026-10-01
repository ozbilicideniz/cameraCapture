#include "mediaviewer.h"
#include <QGridLayout>
#include <QDir>
#include <QPixmap>
#include <QLabel>
#include <QPalette>
#include <QColor>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QDebug>
#include <QPushButton>

MediaViewer::MediaViewer(QWidget *parent)
    : QWidget(parent)
{
    setAutoFillBackground(true);
    setAttribute(Qt::WA_StyledBackground, true);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(35, 35, 35));
    setPalette(pal);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QScrollArea *scrollArea = new QScrollArea(this);
    QWidget *gridContainer = new QWidget();

    mediaGrid = new QGridLayout(gridContainer);

    mediaGrid->setHorizontalSpacing(20);
    mediaGrid->setVerticalSpacing(40);

    scrollArea->setWidget(gridContainer);
    scrollArea->setWidgetResizable(true);
    mainLayout->addWidget(scrollArea);

    QDir photoDir("/root/camera/photos");
    QStringList photos = photoDir.entryList(QStringList() << "*.jpg" << "*.jpeg", QDir::Files);


    int index = 0;

    for (const QString &photoName : photos) {
        QString fullPath = photoDir.filePath(photoName);
        QPixmap pixmap(fullPath);

        QLabel *thumbnail = new QLabel();
        QPushButton *photocard = new QPushButton();
        photocard->setCheckable(true);
        photocard->setFixedSize(280, 190);
        QVBoxLayout *photoLayout = new QVBoxLayout(photocard);
        QLabel *nameLabel = new QLabel(photoName);
        nameLabel->setStyleSheet("color: white;");
        nameLabel->setAlignment(Qt::AlignCenter);
        photoLayout->addWidget(nameLabel);

        photoLayout->addWidget(thumbnail, 0, Qt::AlignHCenter);
        photoLayout->addWidget(nameLabel, 0, Qt::AlignHCenter);

        photocard->setStyleSheet(
            "QPushButton {"
            "    background-color: transparent;"
            "    border: 3px solid transparent;"
            "    padding: 7px;"
            "}"
            "QPushButton:checked {"
            "    background-color: #26384A;"
            "    border: 3px solid #5DADE2;"
            "}"
            );

        QPixmap scaledPixmap = pixmap.scaled(250, 141, Qt::KeepAspectRatio);
        thumbnail->setPixmap(scaledPixmap);

        int row = index / 4;
        int column = index % 4;
        mediaGrid->addWidget(photocard, row, column);
        index++;

    }

}
