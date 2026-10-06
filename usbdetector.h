#ifndef USBDETECTOR_H
#define USBDETECTOR_H

#include <QWidget>
#include <QStringList>
#include <QStackedWidget>
#include "filebrowser.h"

class usbdetector : public QWidget
{
    Q_OBJECT

public:
    explicit usbdetector(QWidget *parent = nullptr);
    QStringList findUsb();
    bool handleBack();

private:
    QStackedWidget *stack = nullptr;
    QWidget *usbPage = nullptr;
    FileBrowser *fileBrowser = nullptr;

    QString usbRoot;
};

#endif // USBDETECTOR_H
