#pragma once

#include "qwebrtc_global.h"
#include <QObject>
#include <QImage>

class QWebRTCMediaTrack;
class QWebRTCVideo_p;
class QWEBRTCSHARED_EXPORT QWebRTCVideo: public QObject {
    Q_OBJECT
    Q_PROPERTY(QObject* videoTrack READ videoTrack WRITE setVideoTrack NOTIFY videoTrackChanged)
    Q_PROPERTY(QSize sourceSize READ sourceSize NOTIFY sourceSizeChanged)
public:
    explicit QWebRTCVideo(QObject *parent = Q_NULLPTR);
    ~QWebRTCVideo();

    QObject* videoTrack();
    void setVideoTrack(QObject*);

	QSize sourceSize();

public Q_SLOTS:
    void asyncUpdate();

Q_SIGNALS:
    void videoTrackChanged();
	void frameChanged(int width,int height,const QByteArray& buffer, QImage::Format format = QImage::Format_ARGB32);
    void sourceSizeChanged();

private:
	QWebRTCVideo_p* m_impl;
};
