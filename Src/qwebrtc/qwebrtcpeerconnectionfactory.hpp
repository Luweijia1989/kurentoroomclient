#pragma once

#include "qwebrtc_global.h"
#include <QObject>
#include <QString>
#include <QSharedPointer>

class QWebRTCPeerConnection;
class QWebRTCPeerConnectionFactory_impl;
class QWebRTCMediaTrack;
class QWebRTCMediaStream;
class QWebRTCConfiguration;

class QWEBRTCSHARED_EXPORT QWebRTCPeerConnectionFactory : public QObject {
public:
    QWebRTCPeerConnectionFactory();
	~QWebRTCPeerConnectionFactory();

    QSharedPointer<QWebRTCMediaTrack> createAudioTrack(const QVariantMap& constraints, const QString& trackId = QString());

    QSharedPointer<QWebRTCMediaTrack> createVideoTrack(const QVariantMap& constraints, const QString& trackId = QString());

    QSharedPointer<QWebRTCMediaTrack> createScreenCaptureTrack(const QVariantMap& constraints, const QString& trackId = QString());

    QSharedPointer<QWebRTCMediaStream> createMediaStream(const QString& label);

    QSharedPointer<QWebRTCPeerConnection> createPeerConnection(const QWebRTCConfiguration&);

	QStringList GetVideoCaputerDevices();
	QStringList GetAudioPlayoutDevices();
	QStringList GetAudioRecordingDevices();

private:
    // This pointer is shared among all peer connections to ensure that all resources allocated by the
    // factory are not deallocated (e.g. the webrtc threads)
    QSharedPointer<QWebRTCPeerConnectionFactory_impl> m_impl;
};
