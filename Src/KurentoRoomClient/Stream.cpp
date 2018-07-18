#include "stdafx.h"
#include "Stream.h"
#include "Room.h"
#include "Participant.h"

#include <qwebrtcpeerconnectionfactory.hpp>
#include <qwebrtcpeerconnection.hpp>
#include <qwebrtcconfiguration.hpp>
#include <qwebrtcmediastream.hpp>
#include <qwebrtcutilities.hpp>
#include <qwebrtcmediatrack.hpp>

const std::string const kStream_Label = "Stream_Label";

namespace kurento
{
	Stream::Stream(KurentoPtr kurento, bool local, Room* room, const Json::Value& options, Participant* peer)
		: kurento(kurento)
		, room(room)
		, local(local)
		, options(options)
		, participant(peer)
	{
		if (options.isMember("id")) {
			id = options["id"].asString();
		}
		else {
			id = "webcam";
		}
	}

	Stream::~Stream()
	{

	}

	std::string Stream::getID() const
	{
		return this->id;
	}

	std::string Stream::getGlobalID() const 
	{
		if (participant) {
			return participant->getID() + "_" + id;
		}
		else {
			return id + "_webcam";
		}
	}

	Participant* Stream::getParticipant()
	{
		return participant;
	}

	void Stream::init()
	{
		participant->addStream(this);

		QSharedPointer<QWebRTCPeerConnectionFactory> factory = kurento->getFactory();
		auto stream = factory->createMediaStream(QString::fromStdString(kStream_Label));
		if (stream.isNull())
		{
			emit("access-denied");
			return;
		}

		_stream = stream;

		if (options.isMember("recvAudio") && options["recvAudio"].asBool())
		{
			QSharedPointer<QWebRTCMediaTrack> audio = factory->createAudioTrack(QVariantMap(),"track_audio");
			if (!audio.isNull())
			{
				_stream->addTrack(audio);
			}
		}

		if (options.isMember("recvVideo") && options["recvVideo"].asBool())
		{
			QSharedPointer<QWebRTCMediaTrack> video = factory->createVideoTrack(QVariantMap(), "track_video");
			if (!video.isNull())
			{
				_stream->addTrack(video);
			}
		}

		if (_stream->tracks().size())
		{
			emit("access-accepted");
		}
		else
		{
			emit("access-denied");
		}
	}

	void Stream::publish()
	{
		initWebRtcPeer(std::bind(&Stream::publishVideoCallback, this, std::placeholders::_1, std::placeholders::_2));
	}

	void Stream::subscribe()
	{
		initWebRtcPeer(std::bind(&Stream::startVideoCallback, this, std::placeholders::_1, std::placeholders::_2));
	}

	void Stream::initWebRtcPeer(CallBack callback)
	{
		auto setLocalDescription = [=](const QSharedPointer<QWebRTCSessionDescription>& desc)
		{
			std::string sdp(desc->sdp().constData());

			_peer->setLocalDescription(desc, [=](bool result)
			{
				callback(result, sdp);
			});
		};

		if (local)
		{
			_peer = kurento->WebRtcPeerSendonly();
			QObject::connect(_peer.data(), &QWebRTCPeerConnection::newIceCandidate,
				[=](const QSharedPointer<QWebRTCIceCandidate>& candidate) {
				participant->sendIceCandidate(candidate);
			});
			
			_peer->addStream(_stream);

			QVariantMap offerOptions;
			_peer->createOfferForConstraints(offerOptions, [=](const QSharedPointer<QWebRTCSessionDescription>& desc)
			{
				setLocalDescription(desc);
			});
		}
		else {

			_peer = kurento->WebRtcPeerRecvonly();
			QObject::connect(_peer.data(), &QWebRTCPeerConnection::streamAdded,
				[=](const QSharedPointer<QWebRTCMediaStream>& stream) {
				_stream = stream;
			});

			QObject::connect(_peer.data(), &QWebRTCPeerConnection::streamRemoved,
				[=](const QSharedPointer<QWebRTCMediaStream>& stream) {
				_stream = nullptr;
			});

			QObject::connect(_peer.data(), &QWebRTCPeerConnection::newIceCandidate,
				[=](const QSharedPointer<QWebRTCIceCandidate>& candidate) {
				participant->sendIceCandidate(candidate);
			});

			QVariantMap offerOptions;
			offerOptions["receiveVideo"] = true;
			offerOptions["receiveAudio"] = true;

			_peer->createOfferForConstraints(offerOptions, [=](const QSharedPointer<QWebRTCSessionDescription>& desc)
			{
				setLocalDescription(desc);
			});
		}
	}

	void Stream::publishVideoCallback(bool result, std::string sdp)
	{
		if (!result)
		{
			return;
		}

		kurento->publishVideo(sdp, false, [=](const Json::Value &result, bool isError)
		{
			Json::FastWriter writer;

			qDebug() << "publishVideo: " << writer.write(result).c_str();

			if (isError) {

			}
			else {
				this->room->emit("stream-published");

				std::string remoteSdp = result["sdpAnswer"].asString();

				processSdpAnswer(remoteSdp);
			}
		});
	}

	void Stream::startVideoCallback(bool result, std::string sdp)
	{
		if (!result)
		{
			return;
		}

		kurento->receiveVideoFrom(getGlobalID(), sdp, [=](const Json::Value &result, bool isError)
		{
			Json::FastWriter writer;

			qDebug() << "receiveVideoFrom: " << writer.write(result).c_str();

			if (isError) {

			}
			else {
				this->room->emit("stream-published");

				std::string remoteSdp = result["sdpAnswer"].asString();

				processSdpAnswer(remoteSdp);
			}
		});
	}

	void Stream::processSdpAnswer(std::string remoteSdp)
	{
		auto type = QWebRTCSessionDescription::SDPType::Answer;
		auto answer = QWebRTCPeerConnection::createSessionDescription(type,
			QString::fromStdString(remoteSdp).toUtf8());

		this->_peer->setRemoteDescription(answer, [=](bool result)
		{
			if (result)
			{
				if (!local)
				{
					room->emit("stream-subscribed",this);
				}
			}
			else
			{
				std::string info = getGlobalID() + ": Error setting SDP to the peer connection: " + remoteSdp;
				qDebug() << info.c_str();
			}
		});
	}
	
	void Stream::addIceCandidate(std::string mId, int sdpMLineIndex, const std::string& sdpData)
	{
		QSharedPointer<QWebRTCIceCandidate> candidate=
			QWebRTCPeerConnection::createIceCandidate(mId.c_str(), sdpMLineIndex, sdpData.c_str());

		if (!candidate.isNull())
		{
			this->_peer->addIceCandidate(candidate);
		}
		else
		{
			std::string info = "Error adding candidate for " + this->id
				+ " stream of endpoint " + participant->getID();

			qCritical() << info.c_str();
		}
	}

	QSharedPointer<QWebRTCMediaStream> Stream::getStream()
	{
		return _stream;
	}
}
