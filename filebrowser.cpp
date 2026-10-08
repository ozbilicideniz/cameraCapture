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

    folderList = new QListWidget();
    mainLayout->addWidget(folderList);

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

    loadDirectory("/");

    connect(folderList, &QListWidget::itemClicked,
            this, [this](QListWidgetItem *item) {

                QDir dir(currentPath);
                QString newPath = dir.filePath(item->text());

                loadDirectory(newPath);
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
            });
}

void FileBrowser::loadDirectory(const QString &path){
    QDir dir(path);
    currentPath = dir.absolutePath();
    pathLabel->setText(currentPath);
    QStringList newList = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    folderList->clear();
    folderList->addItems(newList);
}

void FileBrowser::setFilesToExport(const QStringList &files)
{
    filesToExport = files;
}

QString FileBrowser::getCurrentPath() const
{
    return currentPath;
}

