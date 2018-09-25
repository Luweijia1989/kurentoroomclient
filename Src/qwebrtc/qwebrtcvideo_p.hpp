#pragma once

#include "qwebrtcvideo.hpp"
#include <src/common_video/video_render_frames.h>
//#include <media/base/videosinkinterface.h>
#include "qwebrtcmediatrack_p.hpp"
#include <mutex>
#include <QSharedPointer>


class QWebRTCVideo_p : public rtc::VideoSinkInterface<webrtc::VideoFrame>
{
public:
    explicit QWebRTCVideo_p(QWebRTCVideo*);
    virtual void OnFrame(const webrtc::VideoFrame& frame) override;

	QByteArray m_buffer;
    QWebRTCMediaTrack_impl* m_track;
	QWebRTCVideo *q_ptr;
    std::recursive_mutex videoMutex;
    QSize m_sourceSize;
};
