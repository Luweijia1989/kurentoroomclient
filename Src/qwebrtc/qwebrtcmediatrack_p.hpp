#pragma once

#include "qwebrtcmediatrack.hpp"
#include <src/api/mediastreaminterface.h>

class QWebRTCMediaTrack_impl : public QWebRTCMediaTrack {
    Q_OBJECT
public:
    explicit QWebRTCMediaTrack_impl(const rtc::scoped_refptr<webrtc::VideoTrackInterface>&,
                           rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> videoSource);
    explicit QWebRTCMediaTrack_impl(const rtc::scoped_refptr<webrtc::AudioTrackInterface>&);
    ~QWebRTCMediaTrack_impl();

    virtual QString id() override;

	virtual QString kind() override;

    virtual Type trackType() override;

    //void addVideoSink(const rtc::VideoSinkInterface<webrtc::VideoFrame>&);

    virtual bool screenCast() override;

    virtual void setVolume(uint8_t) override;

	QString m_id;
	QString m_kind;
    Type m_type;
    rtc::scoped_refptr<webrtc::VideoTrackInterface> videoTrack;
    rtc::scoped_refptr<webrtc::AudioTrackInterface> audioTrack;
    rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> videoSource;
};
