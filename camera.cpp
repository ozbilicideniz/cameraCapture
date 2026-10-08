#include "camera.h"
#include "ui_mainwindow.h"
#include <gst/gst.h>
#include <gst/video/videooverlay.h>
#include <gst/wayland/wayland.h>
#include <QDir>
#include <QDebug>
#include <QGuiApplication>
#include <qpa/qplatformnativeinterface.h>
#include <QtWaylandClient/5.12.2/QtWaylandClient/private/wayland-wayland-client-protocol.h>
#include <wayland-client-core.h>
#include <cstring>
#include <gst/app/gstappsink.h>
#include <QFile>
#include <QDateTime>
#include <QEvent>
#include <QFileDialog>
#include "mediaviewer.h"
#include <QTimer>
#include "usbdetector.h"


camera::camera(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mediaViewer = new MediaViewer(this);
    mediaViewer->setGeometry(60, 100, 1800, 800);
    mediaViewer->hide();

    connect(mediaViewer, &MediaViewer::selectionChanged,
            this, [this](int count) {

                ui->exportDestButton->setEnabled(count >= 1);
                ui->previewButton->setEnabled(count == 1);
            });

    usbScreen = new usbdetector(this);
    usbScreen->setGeometry(60, 100, 1800, 800);
    usbScreen->hide();

    ui->exportDestButton->hide();
    ui->backButton->hide();
    ui->previewButton->hide();

    this->setAttribute(Qt::WA_TranslucentBackground);
    ui->centralwidget->setAttribute(Qt::WA_TranslucentBackground);
    ui->cameraWidget->setAttribute(Qt::WA_TranslucentBackground);

    this->setStyleSheet("background: transparent;");
    ui->centralwidget->setStyleSheet("background: transparent;");
    ui->cameraWidget->setStyleSheet("background: transparent;");

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

    ui->cameraButton->setStyleSheet(buttonStyle);
    ui->captureButton->setStyleSheet(buttonStyle);
    ui->recordButton->setStyleSheet(buttonStyle);
    ui->mediaButton->setStyleSheet(buttonStyle);
    ui->exportDestButton->setStyleSheet(buttonStyle);
    ui->backButton->setStyleSheet(buttonStyle);
    ui->previewButton->setStyleSheet(buttonStyle);

    ui->cameraWidget->winId();

    QPlatformNativeInterface *native =
        QGuiApplication::platformNativeInterface();


    void *surface = native->nativeResourceForWindow(
        "surface",
        ui->cameraWidget->windowHandle());


    void *display = native->nativeResourceForIntegration(
        "display");

    wl_display *wlDisplay =
        reinterpret_cast<wl_display *>(display);

    qDebug() << "Wayland display:" << display;
    qDebug() << "Wayland surface:" << surface;

    void *mainSurface = native->nativeResourceForWindow(
        "surface", this->windowHandle());

    qDebug() << "Main Wayland surface:" << mainSurface;


    qDebug() << "Viewfinder windowHandle:"
             << ui->cameraWidget->windowHandle();

    qDebug() << "Main windowHandle:"
             << this->windowHandle();

    qDebug() << "Viewfinder geometry:"
             << ui->cameraWidget->geometry();


    qDebug() << "Viewfinder winId:"
             << ui->cameraWidget->winId();

    qDebug() << "Platform:"
             << QGuiApplication::platformName();

    QDir().mkpath("/root/camera/photos");
    QDir().mkpath("/root/camera/videos");
    QDir().mkpath("/root/camera/videos/thumbnails");

    gst_init(nullptr, nullptr);

    GError *error = nullptr;

    pipeline = gst_parse_launch(
        "v4l2src device=/dev/v4l/by-id/usb-SunplusIT_Inc_YTXB_01.00.00-video-index0 ! "
        "image/jpeg,width=1280,height=720,framerate=30/1 ! "
        "jpegdec ! "
        "tee name=t "

        "t. ! queue ! "
        "waylandsink name=videoSink sync=false "

        "t. ! queue ! "
        "videoconvert ! "
        "jpegenc ! "
        "appsink name=photoSink "
        "max-buffers=1 drop=true sync=false ",

        &error);

    if (error) {
        qDebug() << "GStreamer error:" << error->message;
        g_error_free(error);
        return;
    }

    videoSink = gst_bin_get_by_name(GST_BIN(pipeline), "videoSink");
    GstElement *photoSink = gst_bin_get_by_name(GST_BIN(pipeline), "photoSink");

    tee = gst_bin_get_by_name(GST_BIN(pipeline), "t");

    connect(ui->cameraButton, &QPushButton::clicked,
            this, [this, wlDisplay, mainSurface]()
            {
                qDebug() << "CAMERA BUTTON CLICKED";

                if (!cameraRunning) {
                    qDebug() << "ATTACHING WAYLAND SURFACE";

                    GstContext *context = gst_wayland_display_handle_context_new(wlDisplay);

                    gst_element_set_context(videoSink, context);
                    gst_context_unref(context);

                    gst_video_overlay_set_window_handle(
                        GST_VIDEO_OVERLAY(videoSink),
                        reinterpret_cast<guintptr>(mainSurface));

                    gst_video_overlay_set_render_rectangle(
                        GST_VIDEO_OVERLAY(videoSink),
                        10, 10,
                        1900, 900);

                    qDebug() << "SURFACE ATTACHED";

                    GstStateChangeReturn result =
                        gst_element_set_state(
                            pipeline,
                            GST_STATE_PLAYING);

                    qDebug() << "PLAYING result:" << result;

                    cameraRunning = true;
                    ui->cameraButton->setText("STOP CAMERA");
        }
                else {
                    qDebug() << "STOPPING CAMERA";

                    gst_element_set_state(
                        pipeline,
                        GST_STATE_NULL);

                    cameraRunning = false;

                    ui->cameraButton->setText("START CAMERA");
                    ui->cameraButton->repaint();

                    qDebug() << "CAMERA STOPPED";
                }
            });

    connect(ui->mediaButton, &QPushButton::clicked, this, [this](){

        if(mediaViewer->isVisible()){
            mediaViewer->hide();
            ui->cameraButton->show();
            ui->captureButton->show();
            ui->recordButton->show();
            ui->exportDestButton->hide();
            ui->previewButton->hide();
            ui->backButton->hide();
            ui->mediaButton->show();
        }
        else {
            mediaViewer->show();
            mediaViewer->raise();
            ui->cameraButton->hide();
            ui->captureButton->hide();
            ui->recordButton->hide();
            ui->backButton->show();
            ui->mediaButton->hide();
            ui->exportDestButton->show();
            ui->previewButton->show();

            int count = mediaViewer->getSelectedCount();
            ui->exportDestButton->setEnabled(count >= 1);
            ui->previewButton->setEnabled(count == 1);

        }
    });

    connect(ui->exportDestButton, &QPushButton::clicked,
            this, [this]() {

                usbScreen->sendFilesToBrowser(
                    mediaViewer->getSelectedFiles()
                    );

                usbScreen->show();
                usbScreen->raise();
            });

    connect(ui->previewButton, &QPushButton::clicked,
            this, [this]() {
            mediaViewer->previewPhoto();
            });

    connect(ui->captureButton, &QPushButton::clicked,
            this, [this, photoSink]()
            {
                qDebug() << "TAKE A PICTURE";

                GstSample *photo =
                    gst_app_sink_try_pull_sample(
                        GST_APP_SINK(photoSink),
                        GST_SECOND);

                if (!photo) {
                    qDebug() << "PHOTO: no frame available";
                    return;
                }

                GstBuffer *buffer =
                    gst_sample_get_buffer(photo);

                GstMapInfo map;

                if (!gst_buffer_map(
                        buffer,
                        &map,
                        GST_MAP_READ)) {

                    qDebug() << "PHOTO: could not map buffer";
                    gst_sample_unref(photo);
                    return;
                }

                QString filename = "/root/camera/photos/photo_" +
                QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".jpg";

                QFile file(filename);

                if (file.open(QIODevice::WriteOnly)) {

                    file.write(
                        reinterpret_cast<const char *>(map.data),
                        static_cast<qint64>(map.size));

                    file.close();

                    qDebug() << "PHOTO SAVED:" << filename;
                }
                else {
                    qDebug() << "PHOTO: could not open" << filename;
                }

                gst_buffer_unmap(buffer, &map);
                gst_sample_unref(photo);
            });

    connect(ui->backButton, &QPushButton::clicked,
            this, [this]() {

                if (usbScreen->isVisible()) {

                    if (usbScreen->handleBack()) {
                        return;
                    }

                    usbScreen->hide();
                    mediaViewer->show();
                    mediaViewer->raise();

                    return;
                }

                if (mediaViewer->getCurrentPath() != "/root/camera") {
                    mediaViewer->loadPath("/root/camera");
                    return;
                }

                mediaViewer->hide();

                ui->cameraButton->show();
                ui->captureButton->show();
                ui->recordButton->show();
                ui->mediaButton->show();

                ui->exportDestButton->hide();
                ui->previewButton->hide();
                ui->backButton->hide();
            });

    connect(ui->recordButton, &QPushButton::clicked,
            this, [this, photoSink]()
            {
                if (recording)
                {
                    qDebug() << "STOPPING RECORDING";

                    ui->recordButton->setEnabled(false);
                    ui->recordButton->setText("SAVING...");

                    // Watch fileSink for EOS.
                    // EOS means mp4mux has finished the MP4 file.
                    GstPad *fileSinkPad =
                        gst_element_get_static_pad(
                            recordFileSink,
                            "sink");

                    gst_pad_add_probe(
                        fileSinkPad,
                        GST_PAD_PROBE_TYPE_EVENT_DOWNSTREAM,

                        [](GstPad *,
                           GstPadProbeInfo *info,
                           gpointer userData) -> GstPadProbeReturn
                        {
                            camera *self =
                                static_cast<camera *>(userData);

                            GstEvent *event =
                                GST_PAD_PROBE_INFO_EVENT(info);

                            if (event &&
                                GST_EVENT_TYPE(event) == GST_EVENT_EOS)
                            {
                                qDebug() << "MP4 FINISHED";

                                QMetaObject::invokeMethod(
                                    self,
                                    [self]()
                                    {
                                        qDebug() << "CLEANING OLD RECORDING";

                                        gst_element_set_state(
                                            self->recordQueue,
                                            GST_STATE_NULL);

                                        gst_element_set_state(
                                            self->recordEncoder,
                                            GST_STATE_NULL);

                                        gst_element_set_state(
                                            self->recordParser,
                                            GST_STATE_NULL);

                                        gst_element_set_state(
                                            self->recordMuxer,
                                            GST_STATE_NULL);

                                        gst_element_set_state(
                                            self->recordFileSink,
                                            GST_STATE_NULL);

                                        gst_bin_remove_many(
                                            GST_BIN(self->pipeline),
                                            self->recordQueue,
                                            self->recordEncoder,
                                            self->recordParser,
                                            self->recordMuxer,
                                            self->recordFileSink,
                                            nullptr);

                                        self->recordQueue = nullptr;
                                        self->recordEncoder = nullptr;
                                        self->recordParser = nullptr;
                                        self->recordMuxer = nullptr;
                                        self->recordFileSink = nullptr;

                                        self->recording = false;

                                        self->ui->recordButton->setText("RECORD");
                                        self->ui->recordButton->setEnabled(true);

                                        qDebug() << "READY FOR ANOTHER RECORDING";
                                    },
                                    Qt::QueuedConnection);

                                return GST_PAD_PROBE_REMOVE;
                            }

                            return GST_PAD_PROBE_OK;
                        },
                        this,
                        nullptr);

                    gst_object_unref(fileSinkPad);

                    GstPad *queueSinkPad =
                        gst_element_get_static_pad(
                            recordQueue,
                            "sink");

                    gst_pad_unlink(
                        recordTeePad,
                        queueSinkPad);

                    gst_element_release_request_pad(
                        tee,
                        recordTeePad);

                    gst_object_unref(recordTeePad);
                    recordTeePad = nullptr;

                    gst_object_unref(queueSinkPad);

                    qDebug() << "RECORDING BRANCH DETACHED FROM TEE";

                    GstPad *encoderSinkPad =
                        gst_element_get_static_pad(
                            recordEncoder,
                            "sink");

                    gboolean eosSent =
                        gst_pad_send_event(
                            encoderSinkPad,
                            gst_event_new_eos());

                    gst_object_unref(encoderSinkPad);

                    qDebug() << "EOS SENT TO DETACHED RECORDING BRANCH:"
                             << eosSent;

                    return;
                }


                // ==========================================
                // START RECORDING
                // ==========================================

                qDebug() << "CREATING RECORD QUEUE";

                recordQueue =
                    gst_element_factory_make(
                        "queue",
                        "recordQueue");

                if (!recordQueue)
                {
                    qDebug() << "FAILED TO CREATE RECORD QUEUE";
                    return;
                }

                g_object_set(
                    recordQueue,
                    "leaky", 2,
                    "max-size-buffers", 30,
                    nullptr);

                gst_bin_add(
                    GST_BIN(pipeline),
                    recordQueue);

                qDebug() << "RECORD QUEUE CREATED";


                recordEncoder =
                    gst_element_factory_make(
                        "mpph264enc",
                        "recordEncoder");

                if (!recordEncoder)
                {
                    qDebug() << "FAILED TO CREATE RECORD ENCODER";
                    return;
                }

                gst_bin_add(
                    GST_BIN(pipeline),
                    recordEncoder);

                qDebug() << "RECORD ENCODER ADDED";


                recordParser =
                    gst_element_factory_make(
                        "h264parse",
                        "recordParser");

                if (!recordParser)
                {
                    qDebug() << "FAILED TO CREATE RECORD PARSER";
                    return;
                }

                gst_bin_add(
                    GST_BIN(pipeline),
                    recordParser);

                qDebug() << "RECORD PARSER ADDED";


                recordMuxer =
                    gst_element_factory_make(
                        "mp4mux",
                        "recordMuxer");

                if (!recordMuxer)
                {
                    qDebug() << "FAILED TO CREATE RECORD MUXER";
                    return;
                }

                gst_bin_add(
                    GST_BIN(pipeline),
                    recordMuxer);


                recordFileSink =
                    gst_element_factory_make(
                        "filesink",
                        "recordFileSink");

                if (!recordFileSink)
                {
                    qDebug() << "FAILED TO CREATE RECORD FILESINK";
                    return;
                }

                gst_bin_add(
                    GST_BIN(pipeline),
                    recordFileSink);


                if (!gst_element_link_many(
                        recordQueue,
                        recordEncoder,
                        recordParser,
                        recordMuxer,
                        recordFileSink,
                        nullptr))
                {
                    qDebug() << "FAILED TO LINK RECORDING ELEMENTS";
                    return;
                }

                qDebug() << "RECORDING ELEMENTS LINKED";


                // Create filename
                QString timestamp =
                    QDateTime::currentDateTime()
                        .toString("yyyy-MM-dd_HH-mm-ss");
                QString filename =
                    "/root/camera/videos/" +
                    timestamp +
                    ".mp4";
                QString thumbnailName = "/root/camera/videos/thumbnails/" + timestamp + ".jpg";

                g_object_set(
                    recordFileSink,
                    "location",
                    filename.toUtf8().constData(),
                    nullptr);

                qDebug() << "RECORDING FILE:" << filename;


                // Ask tee for a new output socket
                recordTeePad =
                    gst_element_request_pad_simple(
                        tee,
                        "src_%u");

                // Get recording queue input socket
                GstPad *queueSinkPad =
                    gst_element_get_static_pad(
                        recordQueue,
                        "sink");

                // Connect tee -> recording queue
                if (gst_pad_link(
                        recordTeePad,
                        queueSinkPad) != GST_PAD_LINK_OK)
                {
                    qDebug() << "FAILED TO LINK TEE TO RECORD QUEUE";

                    gst_object_unref(queueSinkPad);
                    return;
                }

                gst_object_unref(queueSinkPad);

                qDebug() << "RECORDING BRANCH CONNECTED TO TEE";


                // Make new recording elements start running
                gst_element_sync_state_with_parent(recordQueue);
                gst_element_sync_state_with_parent(recordEncoder);
                gst_element_sync_state_with_parent(recordParser);
                gst_element_sync_state_with_parent(recordMuxer);
                gst_element_sync_state_with_parent(recordFileSink);


                // We are now recording
                recording = true;

                QTimer::singleShot(500, this, [this, thumbnailName, photoSink]() {
                    GstSample *thumbnail =
                        gst_app_sink_try_pull_sample(
                            GST_APP_SINK(photoSink),
                            GST_SECOND);
                    if (!thumbnail) {
                        qDebug() << "THUMBNAIL GENERATION FAILED";
                        return;
                    }
                    GstBuffer *buffer =
                        gst_sample_get_buffer(thumbnail);
                    GstMapInfo map;

                    if (!gst_buffer_map(
                            buffer,
                            &map,
                            GST_MAP_READ)) {

                        qDebug() << "PHOTO: could not map buffer";
                        gst_sample_unref(thumbnail);
                        return;
                    }

                    QFile file(thumbnailName);

                    if (file.open(QIODevice::WriteOnly)) {

                        file.write(
                            reinterpret_cast<const char *>(map.data),
                            static_cast<qint64>(map.size));

                        file.close();

                        qDebug() << "THUMBNAIL SAVED:" << thumbnailName;
                    }
                    else {
                        qDebug() << "THUMBNAIL: could not open" << thumbnailName;
                    }

                    gst_buffer_unmap(buffer, &map);
                    gst_sample_unref(thumbnail);
                });

                ui->recordButton->setText("STOP VIDEO");

                qDebug() << "RECORDING STARTED";
            });

    QTimer::singleShot(50, this, [this]() {
        ui->cameraButton->parentWidget()->repaint();
    });
}

camera::~camera()
{
    gst_element_set_state(pipeline, GST_STATE_NULL);

    if (videoSink) {
        gst_object_unref(videoSink);
    }

    if (pipeline) {
        gst_object_unref(pipeline);
    }

    delete ui;
}