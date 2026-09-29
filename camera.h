#ifndef CAMERA_H
#define CAMERA_H

#include <QMainWindow>
#include <gst/gst.h>

QT_BEGIN_NAMESPACE

class KeyboardWidget;
class MediaViewer;

namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE



class camera : public QMainWindow
{
    Q_OBJECT

public:
    explicit camera(QWidget *parent = nullptr);
    ~camera() override;

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    Ui::MainWindow *ui;
    KeyboardWidget *keyboard;
    MediaViewer *mediaViewer;

    bool cameraRunning = false;

    GstElement *pipeline = nullptr;
    GstElement *videoSink = nullptr;

    GstElement *recordQueue = nullptr;
    GstElement *recordEncoder = nullptr;
    GstElement *recordParser = nullptr;
    GstElement *recordMuxer = nullptr;
    GstElement *recordFileSink = nullptr;

    GstElement *recordValve = nullptr;

    GstElement *tee = nullptr;
    GstPad *recordTeePad = nullptr;

    bool recording = false;
    bool exportClicked = false;
};

#endif // CAMERA_H
