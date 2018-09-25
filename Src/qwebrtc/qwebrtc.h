#pragma once

#include "qwebrtc_global.h"
#include "qwebrtcpeerconnectionfactory.hpp"

class QWEBRTCSHARED_EXPORT QWebRTC
{

public:
    QWebRTC();

	static void init();
	static void clean();
};

