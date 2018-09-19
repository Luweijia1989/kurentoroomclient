#pragma once

#include "qwebrtcquickvideoitem.hpp"
#include <src/common_video/video_render_frames.h>
#include <api/video/video_sink_interface.h>
#include "qwebrtcmediatrack_p.hpp"
#include <mutex>

class QWebRTCQuickVideoItem_p : public rtc::VideoSinkInterface<webrtc::VideoFrame>
{
public:
    explicit QWebRTCQuickVideoItem_p(QWebRTCQuickVideoItem*);
    virtual void OnFrame(const webrtc::VideoFrame& frame) override;

    QSharedPointer<webrtc::VideoFrame> m_frame;
    QWebRTCMediaTrack_impl* m_track;
    QWebRTCQuickVideoItem *q_ptr;
    std::recursive_mutex videoMutex;
    QSize m_sourceSize;
};
