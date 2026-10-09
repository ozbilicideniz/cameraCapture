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
#include <QFileInfo>
#include <QStackedWidget>
#include "mediaviewer.h"
#include "filebrowser.h"



MediaViewer::MediaViewer(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("MediaViewer { background-color: #232323; }");

    mediaStack = new QStackedWidget(this);

    mediaViewerPage = new QWidget(mediaStack);
    previewPage = new QWidget(mediaStack);

    mediaStack->addWidget(mediaViewerPage);
    mediaStack->addWidget(previewPage);

    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->addWidget(mediaStack);


    QVBoxLayout *mainLayout = new QVBoxLayout(mediaViewerPage);

    QScrollArea *scrollArea = new QScrollArea(mediaViewerPage);
    QWidget *gridContainer = new QWidget();

    mediaGrid = new QGridLayout(gridContainer);

    mediaGrid->setHorizontalSpacing(20);
    mediaGrid->setVerticalSpacing(40);

    scrollArea->setWidget(gridContainer);
    scrollArea->setWidgetResizable(true);

    currentPath = "/root/camera";

    pathLabel = new QLabel(currentPath, mediaViewerPage);

    mainLayout->addWidget(pathLabel);
    mainLayout->addWidget(scrollArea);


    QVBoxLayout *previewLayout = new QVBoxLayout(previewPage);

    previewLabel = new QLabel(previewPage);
    nameLabel = new QLabel(previewPage);

    previewLayout->addWidget(previewLabel);
    previewLayout->addWidget(nameLabel);

    mediaStack->setCurrentWidget(mediaViewerPage);

    loadPath("/root/camera");
}

int MediaViewer::getSelectedCount() const
{
    return selectedFiles.size();
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
            QDir::Files);

        for (const QString &photoName : photos) {

            QString fullPath = photoDir.filePath(photoName);
            QPixmap pixmap(fullPath);

            QLabel *thumbnail = new QLabel();
            QPushButton *photocard = new QPushButton();
            photocard->setCheckable(true);

            if (selectedFiles.contains(fullPath)) {
                photocard->setChecked(true);
            }

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

                        emit selectionChanged(selectedFiles.size());

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

            if (selectedFiles.contains(fullPath)) {
                videocard->setChecked(true);
            }

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

                        emit selectionChanged(selectedFiles.size());
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


QStringList MediaViewer::getSelectedFiles() const
{
    return selectedFiles;
}

QString MediaViewer::getCurrentPath() const
{
    return currentPath;
}

void MediaViewer::previewPhoto()
{
    if (mediaStack->currentWidget() == previewPage) {
        mediaStack->setCurrentWidget(mediaViewerPage);
        loadPath("/root/camera/photos");
        return;
    }

    QString selectedPath = selectedFiles[0];

    QPixmap pixmap(selectedPath);
    previewLabel->setPixmap(pixmap);
    previewLabel->setAlignment(Qt::AlignCenter);

    QFileInfo fileInfo(selectedPath);
    nameLabel->setText(fileInfo.fileName());

    mediaStack->setCurrentWidget(previewPage);
}

void MediaViewer::previewVideo(){
    if (mediaStack->currentWidget() == previewPage) {
        mediaStack->setCurrentWidget(mediaViewerPage);
        loadPath("/root/camera/videos");
        return;
    }

    QString selectedPath = selectedFiles[0];
    QFileInfo fileInfo(selectedPath);
    nameLabel->setText(fileInfo.fileName());

    mediaStack->setCurrentWidget(previewPage);

}

void MediaViewer::deletePhoto()
{
    for (const QString &item : selectedFiles) {
        QString extension = QFileInfo(item).suffix().toLower();

        if (extension == "jpg" || extension == "jpeg") {
            QFile::remove(item);
        }
    }
    selectedFiles.clear();
    loadPath("/root/camera/photos");
}

void MediaViewer::deleteVideo()
{
    for (const QString &item : selectedFiles) {
        QFileInfo info(item);
        QString extension = info.suffix().toLower();

        if (extension == "mp4") {
            QString thumbnail =
                "/root/camera/videos/thumbnails/" +
                info.completeBaseName() + ".jpg";

            QFile::remove(item);
            QFile::remove(thumbnail);
        }
    }

    selectedFiles.clear();
    loadPath("/root/camera/videos");
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