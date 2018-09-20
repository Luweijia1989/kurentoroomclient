#include <memory>
#include <qwebrtcpeerconnectionfactory.hpp>
#include <src/api/peerconnectioninterface.h>
#include "qwebrtcpeerconnection_p.hpp"
#include "qwebrtcpeerconnection.hpp"
#include "qwebrtcmediatrack_p.hpp"
#include "qwebrtcmediastream_p.hpp"
#include "qwebrtcicecandidate.hpp"
#include "qwebrtcconfiguration.hpp"
#include "qwebrtcdatachannel.hpp"
#include "qwebrtcdesktopvideosource_p.hpp"
// #include "src/api/audio_codecs/builtin_audio_decoder_factory.h"
// #include "src/api/audio_codecs/builtin_audio_encoder_factory.h"
#include "src/api/test/fakeconstraints.h"
// #include "api/video_codecs/builtin_video_decoder_factory.h"
// #include "api/video_codecs/builtin_video_encoder_factory.h"
#include "src/media/engine/webrtcvideocapturerfactory.h"
#include "src/modules/audio_device/include/audio_device.h"
#include "src/modules/audio_processing/include/audio_processing.h"
#include "src/modules/video_capture/video_capture_factory.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"
#include "rtc_base/checks.h"
#include "rtc_base/json.h"
#include "rtc_base/logging.h"
#include <QThread>
#include <QCoreApplication>
#include <QMutex>
#include <QWaitCondition>

class QWebRTCPeerConnectionFactory_impl
{
public:

	QWebRTCPeerConnectionFactory_impl()
	{
		m_worker_thread = rtc::Thread::Create();
		if (!m_worker_thread->Start()) {
			qWarning() << "Failed to start worker thread.";
		}

		m_signaling_thread = rtc::Thread::Create();
		if (!m_signaling_thread->Start()) {
			qWarning() << "Failed to start signaling thread.";
		}

		native_interface = webrtc::CreatePeerConnectionFactory(
			nullptr /* network_thread */, m_worker_thread.get() /* worker_thread */,
			m_signaling_thread.get() /* signaling_thread */, nullptr /* default_adm */,
			webrtc::CreateBuiltinAudioEncoderFactory(),
			webrtc::CreateBuiltinAudioDecoderFactory(),
			webrtc::CreateBuiltinVideoEncoderFactory(),
			webrtc::CreateBuiltinVideoDecoderFactory(), nullptr /* audio_mixer */,
			nullptr /* audio_processing */);
	}

public:
	rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> native_interface;
	rtc::scoped_refptr<webrtc::AudioDeviceModule> m_audioDeviceModule;
	std::unique_ptr<rtc::Thread> m_worker_thread;
	std::unique_ptr<rtc::Thread> m_signaling_thread;
	QMutex mutex;
};

//QWebRTCPeerConnectionFactory_impl::~QWebRTCPeerConnectionFactory_impl()
//{
//    m_workerThread->Quit();
//    m_signalingThread->Quit();
//    m_networkingThread->Quit();
//    while (m_workerThread->IsQuitting() || m_signalingThread->IsQuitting() || m_networkingThread->IsQuitting()) {
//        QThread::currentThread()->msleep(50);
//    }
//}

Q_DECLARE_METATYPE(QSharedPointer<QWebRTCIceCandidate>)
Q_DECLARE_METATYPE(QSharedPointer<QWebRTCMediaStream>)
Q_DECLARE_METATYPE(QSharedPointer<QWebRTCDataChannel>)

QWebRTCPeerConnectionFactory::QWebRTCPeerConnectionFactory()
{
	rtc::LogMessage::SetLogToStderr(false);

    qRegisterMetaType<QSharedPointer<QWebRTCIceCandidate> >();
    qRegisterMetaType<QSharedPointer<QWebRTCMediaStream> >();
    qRegisterMetaType<QSharedPointer<QWebRTCDataChannel> >();
    m_impl = QSharedPointer<QWebRTCPeerConnectionFactory_impl>(new QWebRTCPeerConnectionFactory_impl());
}

QSharedPointer<QWebRTCMediaTrack> QWebRTCPeerConnectionFactory::createAudioTrack(const QVariantMap& constraints, const QString& label)
{
	m_impl->mutex.lock();

	if (constraints.contains("recording_device"))
	{
		QString recording_device = constraints["recording_device"].toString();

		int num = m_impl->m_audioDeviceModule->RecordingDevices();
		for (int i = 0; i < num; i++) {
			char name[webrtc::kAdmMaxDeviceNameSize];
			char guid[webrtc::kAdmMaxGuidSize];
			int ret = m_impl->m_audioDeviceModule->RecordingDeviceName(i, name, guid);
			if (ret != -1 && recording_device.compare(name) == 0)
			{
				int result = m_impl->m_audioDeviceModule->SetRecordingDevice(i);
				break;
			}
		}
	}

	if (constraints.contains("playout_device"))
	{
		QString playout_device = constraints["playout_device"].toString();

		int num = m_impl->m_audioDeviceModule->PlayoutDevices();
		for (int i = 0; i < num; i++) {
			char name[webrtc::kAdmMaxDeviceNameSize];
			char guid[webrtc::kAdmMaxGuidSize];
			int ret = m_impl->m_audioDeviceModule->PlayoutDeviceName(i, name, guid);
			if (ret != -1 && playout_device.compare(name) == 0)
			{
				int result = m_impl->m_audioDeviceModule->SetPlayoutDevice(i);
				break;
			}
		}
	}

	m_impl->mutex.unlock();


    auto audioSource = m_impl->native_interface->CreateAudioSource(cricket::AudioOptions());
    auto audioTrack = m_impl->native_interface->CreateAudioTrack(label.toStdString(), audioSource);
    return QSharedPointer<QWebRTCMediaTrack>(new QWebRTCMediaTrack_impl(audioTrack));
}

QSharedPointer<QWebRTCMediaTrack> QWebRTCPeerConnectionFactory::createVideoTrack(const QVariantMap& constraints, const QString& label)
{
    std::vector<std::string> device_names;
    {
        std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> info(
                    webrtc::VideoCaptureFactory::CreateDeviceInfo());
        if (!info) {
            return QSharedPointer<QWebRTCMediaTrack>();
        }
        int num_devices = info->NumberOfDevices();
        for (int i = 0; i < num_devices; ++i) {
            const uint32_t kSize = 256;
            char name[kSize] = {0};
            char id[kSize] = {0};
            if (info->GetDeviceName(i, name, kSize, id, kSize) != -1) {
                device_names.push_back(name);
                qDebug() << "Video device " << i << " " << QString::fromStdString(name);
            }
        }
    }

    cricket::WebRtcVideoDeviceCapturerFactory factory;
    std::unique_ptr<cricket::VideoCapturer> capturer;

	if (constraints.contains("name"))
	{
		QString device_name = constraints["name"].toString();

		capturer = factory.Create(cricket::Device(device_name.toStdString(), 0));
	}
	else
	{
		for (const auto& name : device_names) {
			capturer = factory.Create(cricket::Device(name, 0));
			if (capturer) {
				break;
			}
		}
	}

    if (!capturer) {
        qWarning() << "Could not find a camera device";
        return QSharedPointer<QWebRTCMediaTrack>();
    }
	webrtc::FakeConstraints fakeConstraints;
	if (constraints.contains("minWidth"))
	{
		fakeConstraints.SetMandatoryMinWidth(constraints["minWidth"].toUInt());
	}
	if (constraints.contains("minHeight"))
	{
		fakeConstraints.SetMandatoryMinHeight(constraints["minHeight"].toUInt());
	}
	if (constraints.contains("maxFrameRate"))
	{
		fakeConstraints.SetMandatoryMaxFrameRate(constraints["maxFrameRate"].toUInt());
	}

    auto videoSource = m_impl->native_interface->CreateVideoSource(std::move(capturer), &fakeConstraints);
    auto videoTrack = m_impl->native_interface->CreateVideoTrack(label.toStdString(), videoSource);
    return QSharedPointer<QWebRTCMediaTrack>(new QWebRTCMediaTrack_impl(videoTrack, videoSource));
}

// QSharedPointer<QWebRTCMediaTrack> QWebRTCPeerConnectionFactory::createScreenCaptureTrack(const QVariantMap& constraints, const QString& label)
// {
// 
// 	auto videoSource = new rtc::RefCountedObject<QWebRTCDesktopVideoSource>();
// 	if (constraints.contains("ExcludedWindow"))
// 	{
// 		uint32_t window = constraints["ExcludedWindow"].toUInt();
// 		if (window)
// 		{
// 			videoSource->SetExcludedWindow(window);
// 		}
// 	}
// 
// 	videoSource->moveToThread(QCoreApplication::instance()->thread());
// 	videoSource->Start();
// 	auto videoTrack = m_impl->native_interface->CreateVideoTrack(label.toStdString(), videoSource);
// 	return QSharedPointer<QWebRTCMediaTrack>(new QWebRTCMediaTrack_impl(videoTrack, videoSource));
// }

QSharedPointer<QWebRTCMediaStream> QWebRTCPeerConnectionFactory::createMediaStream(const QString& label)
{
   return QSharedPointer<QWebRTCMediaStream>(new QWebRTCMediaStream_impl(m_impl->native_interface->CreateLocalMediaStream(label.toStdString())));
}

QSharedPointer<QWebRTCPeerConnection> QWebRTCPeerConnectionFactory::createPeerConnection(const QWebRTCConfiguration& config)
{
    //rtc::LogMessage::LogToDebug(rtc::LS_ERROR);
    auto webRTCCOnfig = webrtc::PeerConnectionInterface::RTCConfiguration();
    std::vector<webrtc::PeerConnectionInterface::IceServer> servers;
    for (auto server : config.iceServers) {
        webrtc::PeerConnectionInterface::IceServer iceS;
        iceS.tls_cert_policy = server.tlsCertNoCheck ? webrtc::PeerConnectionInterface::kTlsCertPolicySecure : webrtc::PeerConnectionInterface::kTlsCertPolicyInsecureNoCheck;
        for (auto url : server.urls) {
            iceS.urls.push_back(url.toStdString());
        }
        iceS.username = server.username.toStdString();
        iceS.password = server.password.toStdString();
        servers.push_back(iceS);
    }
    webRTCCOnfig.servers = servers;
	webRTCCOnfig.enable_dtls_srtp = absl::optional<bool>(true);
    auto conn = QSharedPointer<QWebRTCPeerConnection>(new QWebRTCPeerConnection());
    conn->m_impl->m_factory = m_impl;
    conn->m_impl->_conn = m_impl->native_interface->CreatePeerConnection(webRTCCOnfig,
            nullptr, nullptr, conn->m_impl);
    return conn;
}

QStringList QWebRTCPeerConnectionFactory::GetVideoCaputerDevices()
{
	QStringList device_names;
	{
		std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> info(
			webrtc::VideoCaptureFactory::CreateDeviceInfo());
		if (info) {
			int num_devices = info->NumberOfDevices();
			for (int i = 0; i < num_devices; ++i) {
				const uint32_t kSize = 256;
				char name[kSize] = { 0 };
				char id[kSize] = { 0 };
				if (info->GetDeviceName(i, name, kSize, id, kSize) != -1) {
					device_names.push_back(name);
				}
			}
		}
	}

	return device_names;
}

QStringList QWebRTCPeerConnectionFactory::GetAudioPlayoutDevices()
{
	QStringList device_names;

	m_impl->mutex.lock();

	int num = m_impl->m_audioDeviceModule->PlayoutDevices();
	for (int i = 0; i < num; i++) {
		char name[webrtc::kAdmMaxDeviceNameSize];
		char guid[webrtc::kAdmMaxGuidSize];
		int ret = m_impl->m_audioDeviceModule->PlayoutDeviceName(i, name, guid);
		if (ret != -1) {
			device_names.push_back(name);
		}
	}

	m_impl->mutex.unlock();

	return device_names;
}

QStringList QWebRTCPeerConnectionFactory::GetAudioRecordingDevices()
{
	QStringList device_names;

	m_impl->mutex.lock();

	int num = m_impl->m_audioDeviceModule->RecordingDevices();
	for (int i = 0; i < num; i++) {
		char name[webrtc::kAdmMaxDeviceNameSize];
		char guid[webrtc::kAdmMaxGuidSize];
		int ret = m_impl->m_audioDeviceModule->RecordingDeviceName(i, name, guid);
		if (ret != -1) {
			device_names.push_back(name);
		}
	}

	m_impl->mutex.unlock();

	return device_names;
}
