#include "usbdetector.h"
#include "filebrowser.h"
#include "camera.h"
#include <QStorageInfo>
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QDir>


usbdetector::usbdetector(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("usbdetector { background-color: #232323; }");

    stack = new QStackedWidget(this);

    usbPage = new QWidget(stack);
    fileBrowser = new FileBrowser(stack);

    QVBoxLayout *mainLayout = new QVBoxLayout(usbPage);

    stack->addWidget(usbPage);
    stack->addWidget(fileBrowser);

    stack->addWidget(usbPage);
    stack->addWidget(fileBrowser);

    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->addWidget(stack);

    QLabel *titleLabel = new QLabel("SELECT USB DRIVE");
    titleLabel->setStyleSheet(
        "color: white;"
        "font-size: 22px;"
        "font-weight: bold;"
        );

    mainLayout->addWidget(titleLabel);
    QStringList usbPaths = findUsb();
    int index = 0;

    QWidget *gridContainer = new QWidget();
    QGridLayout *usbGrid = new QGridLayout(gridContainer);

    usbGrid->setHorizontalSpacing(20);
    usbGrid->setVerticalSpacing(40);

    mainLayout->addWidget(gridContainer);

    for (const QString &path: usbPaths){

        QPushButton *usbcard = new QPushButton();
        usbcard->setFixedSize(480, 390);
        QVBoxLayout *usbLayout = new QVBoxLayout(usbcard);
        QLabel *nameLabel = new QLabel(path);
        nameLabel->setStyleSheet(
            "color: white;"
            "font-size: 22px;"
            "font-weight: bold;"
            );

        nameLabel->setAlignment(Qt::AlignCenter);

        connect(usbcard, &QPushButton::clicked,
                this, [this, path]() {
            qDebug() << "SELECTED USB:" << path;
            usbRoot = path;
            fileBrowser->loadDirectory(path);
            stack->setCurrentWidget(fileBrowser);
        });

        usbLayout->addWidget(nameLabel, 0, Qt::AlignHCenter);

        usbcard->setStyleSheet(
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

        int row = index / 4;
        int column = index % 4;
        usbGrid->addWidget(usbcard, row, column);
        index++;
    }

    QHBoxLayout *buttonLayout = new QHBoxLayout();

    QPushButton *refreshButton = new QPushButton("REFRESH");
    QPushButton *newFolderButton = new QPushButton("CREATE NEW FOLDER");


    QString buttonStyle =
        "QPushButton {"
        "    background-color: #303030;"
        "    color: white;"
        "    border: 2px solid #707070;"
        "    border-radius: 6px;"
        "    font-size: 18px;"
        "    font-weight: bold;"
        "    padding: 12px;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #505050;"
        "}";


    refreshButton->setStyleSheet(buttonStyle);
    newFolderButton->setStyleSheet(buttonStyle);

    buttonLayout->addWidget(refreshButton);
    buttonLayout->addWidget(newFolderButton);

    mainLayout->addLayout(buttonLayout);


    connect(newFolderButton, &QPushButton::clicked,
            this, [this]() {
                hide();
            });
}

QStringList usbdetector::findUsb()
{
    QStringList usbPaths;

    QList<QStorageInfo> volumes = QStorageInfo::mountedVolumes();

    for (const QStorageInfo &storage : volumes) {

        if (storage.isReady() &&
            !storage.isReadOnly() &&
            storage.rootPath().startsWith("/media/udisk"))
        {
            qDebug() << "USB FOUND:" << storage.rootPath();

            usbPaths.append(storage.rootPath());
        }
    }

    return usbPaths;
}

bool usbdetector::handleBack()
{
    if (stack->currentWidget() == fileBrowser) {

        if (fileBrowser->getCurrentPath() != usbRoot) {

            QDir dir(fileBrowser->getCurrentPath());
            dir.cdUp();

            fileBrowser->loadDirectory(dir.absolutePath());

            return true;
        }

        stack->setCurrentWidget(usbPage);

        return true;
    }

    return false;
}