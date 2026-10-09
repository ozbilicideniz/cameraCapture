#include "filebrowser.h"
#include <QDir>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPalette>
#include <QColor>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include "keyboard.h"
#include <QGridLayout>
#include <QScrollArea>

FileBrowser::FileBrowser(QWidget *parent)
    : QWidget(parent),
    currentPath("/")
{
    setAutoFillBackground(true);
    setAttribute(Qt::WA_StyledBackground, true);

    QPalette pal = palette();
    pal.setColor(QPalette::Window, QColor(35, 35, 35));
    setPalette(pal);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    pathLabel = new QLabel(currentPath);
    mainLayout->addWidget(pathLabel);

    QScrollArea *scrollArea = new QScrollArea(this);
    QWidget *gridContainer = new QWidget();
    fileGrid = new QGridLayout(gridContainer);
    fileGrid->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    scrollArea->setWidget(gridContainer);
    scrollArea->setWidgetResizable(true);

    mainLayout->addWidget(scrollArea);

    folderName = new QLineEdit(this);
    folderName->setPlaceholderText("Folder name");
    folderName->hide();

    mainLayout->addWidget(folderName);

    keyboard = new KeyboardWidget(this);
    keyboard->setGeometry(450, 400, 900, 350);
    keyboard->hide();

    keyboard->setTarget(folderName);

    connect(keyboard, &KeyboardWidget::textConfirmed,
            this, [this](const QString &text) {

                QDir dir(currentPath);

                if (dir.mkdir(text)) {
                    qDebug() << "FOLDER CREATED:" << text;
                    loadDirectory(currentPath);
                }
                else {
                    qDebug() << "COULD NOT CREATE FOLDER:" << text;
                }

                folderName->hide();
            });

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

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *newFolderButton = new QPushButton("NEW FOLDER");
    QPushButton *exportButton = new QPushButton("EXPORT HERE");
    buttonLayout->addWidget(newFolderButton);
    newFolderButton->setStyleSheet(buttonStyle);
    buttonLayout->addWidget(exportButton);
    exportButton->setStyleSheet(buttonStyle);
    mainLayout->addLayout(buttonLayout);


    connect(newFolderButton, &QPushButton::clicked,
            this, [this]() {

                folderName->clear();
                folderName->show();
                folderName->setFocus();

                keyboard->show();
                keyboard->raise();

            });

    connect(exportButton, &QPushButton::clicked,
            this, [this]() {

                for (const QString &sourcePath : filesToExport) {

                    QFileInfo fileInfo(sourcePath);

                    QString destinationPath =
                        QDir(currentPath).filePath(fileInfo.fileName());

                    qDebug() << "COPY:"
                             << sourcePath
                             << "->"
                             << destinationPath;

                    if (QFile::copy(sourcePath, destinationPath)) {
                        qDebug() << "EXPORTED:" << destinationPath;
                    }
                    else {
                        qDebug() << "EXPORT FAILED:" << destinationPath;
                    }
                }
                loadDirectory(currentPath);
            });
}

void FileBrowser::loadDirectory(const QString &path){
    QDir dir(path);
    currentPath = dir.absolutePath();
    pathLabel->setText(currentPath);
    QStringList newList = dir.entryList(QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot);
    clearGrid();

    int index = 0;

    for (const QString &name : newList) {
        QString fullPath = dir.filePath(name);
        QFileInfo info(fullPath);
        if (info.isDir()){
            QPushButton *folderCard = new QPushButton();
            folderCard->setFixedSize(280, 190);

            QVBoxLayout *folderLayout = new QVBoxLayout(folderCard);

            QLabel *folderIcon = new QLabel();
            QLabel *nameLabel = new QLabel(name);

            QPixmap folderPixmap(":/icons/folder.png");
            QPixmap scaledFolderPixmap =
                folderPixmap.scaled(120, 120, Qt::KeepAspectRatio);

            folderIcon->setPixmap(scaledFolderPixmap);
            folderIcon->setAlignment(Qt::AlignCenter);

            nameLabel->setStyleSheet("color: white;");
            nameLabel->setAlignment(Qt::AlignCenter);

            folderLayout->addWidget(folderIcon, 0, Qt::AlignHCenter);
            folderLayout->addWidget(nameLabel, 0, Qt::AlignHCenter);

            folderCard->setStyleSheet(
                "QPushButton {"
                "    background-color: transparent;"
                "    border: 3px solid transparent;"
                "    padding: 7px;"
                "}"
                "QPushButton:pressed {"
                "    background-color: #303030;"
                "    border: 3px solid #707070;"
                "}"
                );
            connect(folderCard, &QPushButton::clicked,
                    this, [this, fullPath]() {
                        loadDirectory(fullPath);
                    });
            int row = index / 6;
            int column = index % 6;

            fileGrid->addWidget(folderCard, row, column);
            index++;

        }
        else if (info.isFile()){
            QString extension = info.suffix().toLower();

            if (extension == "jpg" || "jpeg"){

                QPixmap pixmap(fullPath);
                QLabel *thumbnail = new QLabel();
                QPushButton *photocard = new QPushButton();
                photocard->setFixedSize(280, 190);
                QVBoxLayout *photoLayout = new QVBoxLayout(photocard);
                QLabel *nameLabel = new QLabel(name);
                nameLabel->setStyleSheet("color: white;");
                nameLabel->setAlignment(Qt::AlignCenter);
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

                int row = index / 6;
                int column = index % 6;
                fileGrid->addWidget(photocard, row, column);
                index++;

            }
            else if (extension == "mp4"){

            }
        }
    }
}

void FileBrowser::setFilesToExport(const QStringList &files)
{
    filesToExport = files;
}

QString FileBrowser::getCurrentPath() const
{
    return currentPath;
}

void FileBrowser::clearGrid()
{
    while (fileGrid->count() > 0) {

        QLayoutItem *item = fileGrid->takeAt(0);

        if (QWidget *widget = item->widget()) {
            delete widget;
        }

        delete item;
    }
}
