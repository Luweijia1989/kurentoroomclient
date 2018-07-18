#pragma once

#include "qwebrtc_global.h"
#include <memory>
#include <QString>

class QWebRTCSessionDescription_impl;

class QWEBRTCSHARED_EXPORT QWebRTCSessionDescription {
public:
    enum class SDPType {
        Offer,
        PrAnswer,
        Answer
    };

    virtual SDPType type() const = 0;
    virtual QByteArray sdp() const = 0;
    virtual bool isValid() const = 0;

    static std::string SDPTypeToString(QWebRTCSessionDescription::SDPType);
};
