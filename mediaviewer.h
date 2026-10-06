#ifndef MEDIAVIEWER_H
#define MEDIAVIEWER_H

#include <QWidget>
#include <QLabel>
#include <QString>

class QGridLayout;

class MediaViewer : public QWidget
{
    Q_OBJECT

public:
    explicit MediaViewer(QWidget *parent = nullptr);
    QString getCurrentPath() const;
    void loadPath(const QString &path);
    int getSelectedCount() const;

signals:
    void selectionChanged(int count);

private:
    QGridLayout *mediaGrid;
    QLabel *pathLabel;
    QString currentPath;
    void clearGrid();
    QStringList selectedFiles;
};

#endif
