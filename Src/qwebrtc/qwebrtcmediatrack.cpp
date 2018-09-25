#include "qwebrtcmediatrack_p.hpp"
#include "src/api/mediaconstraintsinterface.h"

namespace QMediaTrackConstraints {
    const QString minAspectRatio = (webrtc::MediaConstraintsInterface::kMinAspectRatio);
    const QString maxAspectRatio = (webrtc::MediaConstraintsInterface::kMaxAspectRatio);
    const QString maxWidth = (webrtc::MediaConstraintsInterface::kMaxWidth);
    const QString minWidth = (webrtc::MediaConstraintsInterface::kMinWidth);
    const QString maxHeight = (webrtc::MediaConstraintsInterface::kMaxHeight);
    const QString minHeight = (webrtc::MediaConstraintsInterface::kMinHeight);
    const QString maxFrameRate = (webrtc::MediaConstraintsInterface::kMaxFrameRate);
    const QString minFrameRate = (webrtc::MediaConstraintsInterface::kMinFrameRate);
    //const QString levelControl = (webrtc::MediaConstraintsInterface::kLevelControl);

    const QString valueTrue = (webrtc::MediaConstraintsInterface::kValueTrue);
    const QString valueFalse = (webrtc::MediaConstraintsInterface::kValueFalse);
}

QWebRTCMediaTrack_impl::QWebRTCMediaTrack_impl(const rtc::scoped_refptr<webrtc::VideoTrackInterface>& track,
        rtc::scoped_refptr<webrtc::VideoTrackSourceInterface> videoSource)
    : m_id(QString::fromStdString(track->id())), m_kind("video"),m_type(QWebRTCMediaTrack::Type::Video), videoTrack(track), videoSource(videoSource)
{
}

QWebRTCMediaTrack_impl::QWebRTCMediaTrack_impl(const rtc::scoped_refptr<webrtc::AudioTrackInterface>& track)
    : m_id(QString::fromStdString(track->id())), m_kind("audio"), m_type(QWebRTCMediaTrack::Type::Audio), audioTrack(track)
{
}

QWebRTCMediaTrack_impl::~QWebRTCMediaTrack_impl()
{

}

QString QWebRTCMediaTrack_impl::id()
{
	return m_id;
}

QString QWebRTCMediaTrack_impl::kind()
{
	return m_kind;
}

QWebRTCMediaTrack::Type QWebRTCMediaTrack_impl::trackType()
{
    return m_type;
}

bool QWebRTCMediaTrack_impl::screenCast()
{
    if (videoSource && videoSource->is_screencast()) {
        return true;
    }
    return false;
}

void QWebRTCMediaTrack_impl::setVolume(uint8_t volume)
{
    if (audioTrack && volume <= 10) {
        if (auto audioSource = audioTrack->GetSource()) {
            audioSource->SetVolume(volume);
        }
    }
}
