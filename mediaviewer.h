#ifndef MEDIAVIEWER_H
#define MEDIAVIEWER_H

#include <QWidget>

class QGridLayout;

class MediaViewer : public QWidget
{
    Q_OBJECT

public:
    explicit MediaViewer(QWidget *parent = nullptr);

private:
    QGridLayout *mediaGrid;
};

#endif
