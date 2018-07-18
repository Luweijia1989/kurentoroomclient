#include "qwebrtcsessiondescription.hpp"
#include <api/jsep.h>
#include <QSharedPointer>

class QWebRTCSessionDescription_impl {
public:
    QWebRTCSessionDescription_impl(QWebRTCSessionDescription::SDPType type, const QByteArray& sdp)
        : m_type(type), m_sdp(sdp)
    {
		m_sessionDescription = webrtc::CreateSessionDescription(type == QWebRTCSessionDescription::SDPType::Answer ? "answer" : "offer",
			sdp.toStdString(), nullptr);
    }

    QWebRTCSessionDescription::SDPType m_type;
    QByteArray m_sdp;
	webrtc::SessionDescriptionInterface* m_sessionDescription;
};
