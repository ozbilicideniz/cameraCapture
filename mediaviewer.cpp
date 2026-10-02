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
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("MediaViewer { background-color: #232323; }");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QScrollArea *scrollArea = new QScrollArea(this);
    QWidget *gridContainer = new QWidget();

    mediaGrid = new QGridLayout(gridContainer);

    mediaGrid->setHorizontalSpacing(20);
    mediaGrid->setVerticalSpacing(40);

    scrollArea->setWidget(gridContainer);
    scrollArea->setWidgetResizable(true);

    currentPath = "/root/camera";
    pathLabel = new QLabel(currentPath);

    mainLayout->addWidget(pathLabel);
    mainLayout->addWidget(scrollArea);

    loadPath("/root/camera");

}

void MediaViewer::loadPath(const QString &path)
{
    currentPath = path;
    pathLabel->setText(currentPath);

    clearGrid();

    if (currentPath == "/root/camera") {

        QPushButton *photosButton = new QPushButton("Photos");
        QPushButton *videosButton = new QPushButton("Videos");

        QString buttonStyle =
            "QPushButton {"
            "    background-color: #303030;"
            "    color: white;"
            "    border: 2px solid #707070;"
            "    border-radius: 6px;"
            "    font-size: 18px;"
            "    font-weight: bold;"
            "}"
            "QPushButton:pressed {"
            "    background-color: #505050;"
            "}";

        photosButton->setStyleSheet(buttonStyle);
        videosButton->setStyleSheet(buttonStyle);

        photosButton->setIcon(QIcon(":/icons/folder.png"));
        photosButton->setIconSize(QSize(120, 120));

        videosButton->setIcon(QIcon(":/icons/folder.png"));
        videosButton->setIconSize(QSize(120, 120));


        mediaGrid->addWidget(photosButton, 0, 0);
        mediaGrid->addWidget(videosButton, 0, 1);


        connect(photosButton, &QPushButton::clicked,
                this, [this]() {
                    loadPath("/root/camera/photos");
                });

        connect(videosButton, &QPushButton::clicked,
                this, [this]() {
                    loadPath("/root/camera/videos");
                });
    }

    else if (currentPath == "/root/camera/photos") {

        int index = 0;

        QDir photoDir("/root/camera/photos");

        QStringList photos = photoDir.entryList(
            QStringList() << "*.jpg" << "*.jpeg",
            QDir::Files
            );

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

            connect(photocard, &QPushButton::toggled,
                    this, [this, fullPath](bool checked) {

                        if (checked) {
                            qDebug() << "SELECTED:" << fullPath;
                            selectedFiles.append(fullPath);
                        }
                        else {
                            qDebug() << "DESELECTED:" << fullPath;
                            selectedFiles.removeAll(fullPath);
                        }

                    });


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
    else if (currentPath == "/root/camera/videos") {

        int index = 0;

        QDir videoDir("/root/camera/videos");

        QStringList videos = videoDir.entryList(
            QStringList() << "*.mp4",
            QDir::Files
            );

        for (const QString &videoName : videos) {

            QString fullPath = videoDir.filePath(videoName);

            QPushButton *videocard = new QPushButton();
            videocard->setCheckable(true);
            videocard->setFixedSize(280, 190);
            QVBoxLayout *videoLayout = new QVBoxLayout(videocard);
            QLabel *nameLabel = new QLabel(videoName);
            nameLabel->setStyleSheet("color: white;");
            nameLabel->setAlignment(Qt::AlignCenter);

            connect(videocard, &QPushButton::toggled,
                    this, [this, fullPath](bool checked) {

                        if (checked) {
                            qDebug() << "SELECTED:" << fullPath;
                            selectedFiles.append(fullPath);
                        }
                        else {
                            qDebug() << "DESELECTED:" << fullPath;
                            selectedFiles.removeAll(fullPath);
                        }

                    });

            videocard->setStyleSheet(
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

            QLabel *thumbnail = new QLabel("VIDEO");
            thumbnail->setFixedSize(250, 141);
            thumbnail->setAlignment(Qt::AlignCenter);
            thumbnail->setStyleSheet(
                "background-color: #111111;"
                "color: white;"
                "font-size: 30px;"
                );

            videoLayout->addWidget(thumbnail, 0, Qt::AlignHCenter);
            videoLayout->addWidget(nameLabel, 0, Qt::AlignHCenter);

            int row = index / 4;
            int column = index % 4;
            mediaGrid->addWidget(videocard, row, column);
            index++;

        }
    }
}

QString MediaViewer::getCurrentPath() const
{
    return currentPath;
}

void MediaViewer::clearGrid()
{
    while (mediaGrid->count() > 0) {

        QLayoutItem *item = mediaGrid->takeAt(0);

        if (QWidget *widget = item->widget()) {
            delete widget;
        }

        delete item;
    }
}