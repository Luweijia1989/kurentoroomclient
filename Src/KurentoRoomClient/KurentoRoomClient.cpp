#include "stdafx.h"
#include "KurentoRoomClient.h"
#include <json/json.h>
#include "Room.h"
#include "Stream.h"
#include "Participant.h"

#include <qwebrtc.h>
#include <qwebrtcpeerconnectionfactory.hpp>
#include <qwebrtcpeerconnection.hpp>
#include <qwebrtcconfiguration.hpp>

#pragma comment(lib,"qwebrtc.lib")


namespace kurento
{
	KurentoRoomClient::KurentoRoomClient(std::string wsUri)
		: _wsUri(wsUri)
	{
		_handler = std::make_shared<kurento::JsonRpc::Handler>();
		_transport = std::make_shared<WebSocketTransport>();
		_client = std::make_shared<kurento::JsonRpc::Client>(_transport, _handler);

		_handler->addMethod("iceCandidate", std::bind(&KurentoRoomClient::oniceCandidate,
			this, std::placeholders::_1, std::placeholders::_2));

		_handler->addMethod("participantJoined", std::bind(&KurentoRoomClient::onParticipantJoined,
			this, std::placeholders::_1, std::placeholders::_2));

		_handler->addMethod("participantPublished", std::bind(&KurentoRoomClient::onParticipantPublished,
			this, std::placeholders::_1, std::placeholders::_2));

		_handler->addMethod("participantUnpublished", std::bind(&KurentoRoomClient::onParticipantLeft,
			this, std::placeholders::_1, std::placeholders::_2));

		_handler->addMethod("participantLeft", std::bind(&KurentoRoomClient::onParticipantLeft,
			this, std::placeholders::_1, std::placeholders::_2));

		_handler->addMethod("participantEvicted", std::bind(&KurentoRoomClient::onParticipantEvicted,
			this, std::placeholders::_1, std::placeholders::_2));

		_handler->addMethod("sendMessage", std::bind(&KurentoRoomClient::onNewMessage,
			this, std::placeholders::_1, std::placeholders::_2));

		_factory.reset(QWebRTC::init());
	}

	KurentoRoomClient::~KurentoRoomClient()
	{
// 		_peerSend.reset();
// 		_peerRecv.reset();
		_factory.reset();
	}

	Room* KurentoRoomClient::room(const Options& options)
	{
		if (!_room)
		{
			_room = new Room(shared_from_this(), options);
		}

		return _room;
	}

	Stream* KurentoRoomClient::stream(Room* room, bool audio, bool video, bool data)
	{
		Json::Value options;
		options["recvVideo"] = video;
		options["recvAudio"] = audio;
		options["data"] = data;

		Stream* stream = new Stream(shared_from_this(), true, room, options, _room->getLocalParticipant());
		return stream;
	}

	QSharedPointer<QWebRTCPeerConnectionFactory> KurentoRoomClient::getFactory()
	{
		if (_factory.isNull())
		{
			_factory.reset(new QWebRTCPeerConnectionFactory);
		}

		return _factory;
	}

	QSharedPointer<QWebRTCPeerConnection> KurentoRoomClient::WebRtcPeerSendonly()
	{
// 		if (_peerSend.isNull())
// 		{
// 			
// 			_peerSend = getFactory()->createPeerConnection(configuration);
// 		}
		QWebRTCConfiguration configuration;
		return getFactory()->createPeerConnection(configuration);
	}

	QSharedPointer<QWebRTCPeerConnection> KurentoRoomClient::WebRtcPeerRecvonly()
	{
// 		if (_peerRecv.isNull())
// 		{
// 			QWebRTCConfiguration configuration;
// 			_peerRecv = getFactory()->createPeerConnection(configuration);
// 		}
// 
// 		return _peerRecv;
		QWebRTCConfiguration configuration;
		return getFactory()->createPeerConnection(configuration);
	}

	void KurentoRoomClient::joinRoom(std::string roomName, std::string userName, bool dataChannels, Continuation cont)
	{
		auto joinRoom = [=]()
		{
			Json::Value params;
			params["user"] = userName;
			params["room"] = roomName;

			_client->sendRequest("joinRoom", params, cont);
		};

		_transport->once("onconnected", joinRoom);

		_transport->doconnect(_wsUri);
	}

	void KurentoRoomClient::leaveRoom(Continuation cont)
	{
		Json::Value params;
		_client->sendRequest("leaveRoom", params, cont);
	}

	void KurentoRoomClient::publishVideo(std::string sdpOffer, bool doLoopback, Continuation cont) {

		Json::Value params;
		params["sdpOffer"] = sdpOffer;
		params["doLoopback"] = doLoopback;

		_client->sendRequest("publishVideo", params, cont);
	}

	void KurentoRoomClient::unpublishVideo()
	{
		Json::Value params;
		_client->sendRequest("unpublishVideo", params);
	}

	// sender should look like 'username_streamId'
	void KurentoRoomClient::receiveVideoFrom(std::string sender, std::string sdpOffer, Continuation cont)
	{
		Json::Value params;
		params["sender"] = sender;
		params["sdpOffer"] = sdpOffer;

		_client->sendRequest("receiveVideoFrom", params, cont);
	}

	// sender should look like 'username_streamId'
	void KurentoRoomClient::unsubscribeFromVideo(std::string sender)
	{
		Json::Value params;
		params["sender"] = sender;

		_client->sendRequest("unsubscribeFromVideo", params);
	}

	void KurentoRoomClient::onIceCandidate(std::string endpointName, std::string candidate, std::string sdpMid, int sdpMLineIndex, Continuation cont)
	{
		Json::Value params;
		params["endpointName"] = endpointName;
		params["candidate"] = candidate;
		params["sdpMid"] = sdpMid;
		params["sdpMLineIndex"] = sdpMLineIndex;

		_client->sendRequest("onIceCandidate", params, cont);
	}

	void KurentoRoomClient::sendMessage(std::string userName, std::string roomName, std::string message, Continuation cont)
	{
		Json::Value params;
		params["userMessage"] = userName;
		params["roomMessage"] = roomName;
		params["message"] = message;

		_client->sendRequest("sendMessage", params, cont);
	}

	void KurentoRoomClient::customRequest(Json::Value& customReqParams)
	{
		return _client->sendRequest("customRequest", customReqParams);
	}

	void KurentoRoomClient::oniceCandidate(const Json::Value& params, Json::Value& responce)
	{
		if (isRoomAvailable())
		{
			_room->recvIceCandidate(params);
		}
	}

	bool KurentoRoomClient::isRoomAvailable()
	{
		return _room;
	}

	void KurentoRoomClient::onParticipantJoined(const Json::Value& params, Json::Value& responce)
	{
		if (isRoomAvailable()) {
			_room->onParticipantJoined(params);
		}
	}

	void KurentoRoomClient::onParticipantPublished(const Json::Value& params, Json::Value& responce)
	{
		if (isRoomAvailable()) {
			_room->onParticipantPublished(params);
		}
	}
	void KurentoRoomClient::onParticipantLeft(const Json::Value& params, Json::Value& responce)
	{
		if (isRoomAvailable()) {
			_room->onParticipantLeft(params);
		}
	}

	void KurentoRoomClient::onParticipantEvicted(const Json::Value& params, Json::Value& responce)
	{
		if (isRoomAvailable()) {
			_room->onParticipantEvicted(params);
		}
	}

	void KurentoRoomClient::onNewMessage(const Json::Value& params, Json::Value& responce)
	{
		if (isRoomAvailable()) {
			_room->onNewMessage(params);
		}
	}
}
