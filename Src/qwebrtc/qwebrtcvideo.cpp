#include "qwebrtcvideo_p.hpp"
#include <QDebug>
#include <assert.h>
#include <webrtc/common_video/libyuv/include/webrtc_libyuv.h>
#include <QMutex>
#include <QElapsedTimer>

QWebRTCVideo::QWebRTCVideo(QObject *parent)
    : QObject(parent), m_impl(new QWebRTCVideo_p(this))
{

}

QWebRTCVideo::~QWebRTCVideo()
{
	{
		std::lock_guard<std::recursive_mutex> locker(m_impl->videoMutex);
		m_impl->q_ptr = 0;
		if (m_impl->m_track && m_impl->m_track->videoTrack) {
			m_impl->m_track->videoTrack->RemoveSink(m_impl);
		}
	}

    delete m_impl;
}

QObject* QWebRTCVideo::videoTrack()
{
    std::lock_guard<std::recursive_mutex> locker(m_impl->videoMutex);
    return m_impl->m_track;
}

void QWebRTCVideo::setVideoTrack(QObject* track)
{
    std::lock_guard<std::recursive_mutex> locker(m_impl->videoMutex);
    m_impl->m_sourceSize = QSize();
    auto trackImpl = qobject_cast<QWebRTCMediaTrack_impl*>(track);
    if (!trackImpl) {
        m_impl->m_buffer.resize(0);
        //qWarning() << "incompatible object assigned as video track";
        qDebug() << "Video track changed to " << (long long)trackImpl;
        Q_EMIT videoTrackChanged();
        return;
    }
    assert(trackImpl->trackType() == QWebRTCMediaTrack::Type::Video);
    m_impl->m_track = trackImpl;
    m_impl->m_track->videoTrack->AddOrUpdateSink(m_impl, rtc::VideoSinkWants());
    Q_EMIT videoTrackChanged();
}

QSize QWebRTCVideo::sourceSize()
{
    std::lock_guard<std::recursive_mutex> locker(m_impl->videoMutex);
    return m_impl->m_sourceSize;
}

void QWebRTCVideo::asyncUpdate()
{
	std::lock_guard<std::recursive_mutex> locker(m_impl->videoMutex);
	int width = m_impl->m_sourceSize.width();
	int height = m_impl->m_sourceSize.height();

	Q_EMIT frameChanged(width, height, m_impl->m_buffer, QImage::Format_ARGB32);
}

QWebRTCVideo_p::QWebRTCVideo_p(QWebRTCVideo* q)
    : m_track(0), q_ptr(q)
{
}

void QWebRTCVideo_p::OnFrame(const webrtc::VideoFrame& frame)
{
    std::lock_guard<std::recursive_mutex> locker(videoMutex);
    if (m_sourceSize.width() != frame.width() || m_sourceSize.height() != frame.height()) {
        m_sourceSize = QSize(frame.width(), frame.height());
        Q_EMIT q_ptr->sourceSizeChanged();
    }

	m_buffer.resize(frame.width()*frame.height() * 4);
	webrtc::ConvertFromI420(frame, webrtc::VideoType::kARGB, 0, (uint8_t*)m_buffer.data());

    QMetaObject::invokeMethod(q_ptr, "asyncUpdate");
}
