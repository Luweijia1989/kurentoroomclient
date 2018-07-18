#pragma once

#include <QVariantMap>
#include <QSharedPointer>
#include "qwebrtcsessiondescription.hpp"
#include "qwebrtcicecandidate.hpp"

namespace QWebRTCUtilities {
QWEBRTCSHARED_EXPORT QSharedPointer<QWebRTCSessionDescription> sessionFromJSON(const QJsonObject&);
QWEBRTCSHARED_EXPORT QSharedPointer<QWebRTCIceCandidate> iceCandidateFromJSON(const QJsonObject&);
QWEBRTCSHARED_EXPORT QJsonObject iceCandidateToJSON(const QSharedPointer<QWebRTCIceCandidate>&);
QWEBRTCSHARED_EXPORT QJsonObject sessionToJSON(const QSharedPointer<QWebRTCSessionDescription>&);
}
