#include "qwebrtc.h"

#include "rtc_base/checks.h"
#include "rtc_base/ssladapter.h"
#include "rtc_base/win32socketinit.h"
#include "rtc_base/win32socketserver.h"

#ifdef _DEBUG
#pragma comment(lib,"qtmaind.lib")
#pragma comment(lib,"Qt5Cored.lib")
// #pragma comment(lib,"Qt5OpenGLd.lib")
// #pragma comment(lib,"Qt5Guid.lib")
#else
#pragma comment(lib,"qtmain.lib")
#pragma comment(lib,"Qt5Core.lib")
// #pragma comment(lib,"Qt5OpenGL.lib")
// #pragma comment(lib,"Qt5Gui.lib")
#endif 

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"Winmm.lib")
#pragma comment(lib,"Strmiids.lib")
#pragma comment(lib,"Msdmo.lib")
#pragma comment(lib,"Dmoguids.lib")
#pragma comment(lib,"Secur32.lib")
#pragma comment(lib,"wmcodecdspuuid.lib")



static rtc::Win32Thread *w32_thread = nullptr;
static rtc::Win32SocketServer *w32_ss = nullptr;

QWebRTC::QWebRTC()
{
}

void QWebRTC::init()
{
// 	if (!w32_thread)
// 	{
// 		w32_ss = new rtc::Win32SocketServer;
// 		w32_thread = new rtc::Win32Thread(w32_ss);
// 	}
	rtc::EnsureWinsockInit();
//	rtc::ThreadManager::Instance()->SetCurrentThread(w32_thread);
	
	rtc::InitializeSSL();
}
