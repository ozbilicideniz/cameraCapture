#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <QWidget>

class QListWidget;
class QLabel;

class FileBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit FileBrowser(QWidget *parent = nullptr);
    void loadDirectory(const QString &path);
    QString getCurrentPath() const;

private:
    QLabel *pathLabel;
    QListWidget *folderList;
    QString currentPath;
    QString usbRoot;
};

#endif