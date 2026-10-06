#include "filebrowser.h"
#include <QDir>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPalette>
#include <QColor>


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
    QPushButton *cancelButton = new QPushButton("CANCEL");
    QPushButton *exportButton = new QPushButton("EXPORT HERE");
    buttonLayout->addWidget(cancelButton);
    cancelButton->setStyleSheet(buttonStyle);
    buttonLayout->addWidget(exportButton);
    exportButton->setStyleSheet(buttonStyle);
    mainLayout->addLayout(buttonLayout);


    connect(cancelButton, &QPushButton::clicked,
            this, [this]() {
                hide();
            });

    connect(exportButton, &QPushButton::clicked,
            this, [this]() {

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

QString FileBrowser::getCurrentPath() const
{
    return currentPath;
}