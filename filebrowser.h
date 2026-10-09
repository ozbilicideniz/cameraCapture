#ifndef FILEBROWSER_H
#define FILEBROWSER_H

#include <QWidget>
#include <QLineEdit>
#include "keyboard.h"

class QListWidget;
class QLabel;
class QGridLayout;

class FileBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit FileBrowser(QWidget *parent = nullptr);
    void loadDirectory(const QString &path);
    QString getCurrentPath() const;
    void setFilesToExport(const QStringList &files);
    void clearGrid();

private:
    QLabel *pathLabel;
    QListWidget *folderList;
    QString currentPath;
    QString usbRoot;
    QStringList filesToExport;
    QLineEdit *folderName;
    KeyboardWidget *keyboard;
    QGridLayout *fileGrid;

};

#endif