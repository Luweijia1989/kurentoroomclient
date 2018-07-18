#pragma once

#undef emit
#include <EventEmitter.hpp>
#include "WebSocketTransport.h"

class QWebRTCPeerConnection;
class QWebRTCPeerConnectionFactory;

namespace kurento
{
	struct Options
	{
		std::string room;
		std::string user;
		bool subscribeToStreams = true;
		int updateSpeakerInterval = 1500;
		int thresholdSpeaker = -50;
	};

	using Continuation = JsonRpc::Client::Continuation;

	class KurentoRoomClient : /*public EventEmitter,*/ public std::enable_shared_from_this<KurentoRoomClient>
	{
		friend class Room;
		friend class Stream;
		friend class Participant;
	public:
		KurentoRoomClient(std::string wsUri);
		~KurentoRoomClient();

	public:
		Room* room(const Options& options);
		Stream* stream(Room* room, bool audio,bool video,bool data);

	protected:
		QSharedPointer<QWebRTCPeerConnectionFactory> getFactory();
		QSharedPointer<QWebRTCPeerConnection> WebRtcPeerSendonly();
		QSharedPointer<QWebRTCPeerConnection> WebRtcPeerRecvonly();

	public:
		void joinRoom(std::string roomName, std::string userName, bool dataChannels, Continuation cont);
		void leaveRoom(Continuation cont);
		void publishVideo(std::string sdpOffer, bool doLoopback, Continuation cont);
		void unpublishVideo();
		// sender should look like 'username_streamId'
		void receiveVideoFrom(std::string sender, std::string sdpOffer, Continuation cont);
		// sender should look like 'username_streamId'
		void unsubscribeFromVideo(std::string sender);
		void onIceCandidate(std::string endpointName, std::string candidate, std::string sdpMid, int sdpMLineIndex, Continuation cont);
		void sendMessage(std::string userName, std::string roomName, std::string message, Continuation cont);
		void customRequest(Json::Value& customReqParams);

		void oniceCandidate(const Json::Value &, Json::Value &);
		void onParticipantJoined(const Json::Value &, Json::Value &);
		void onParticipantPublished(const Json::Value &, Json::Value &);
		void onParticipantLeft(const Json::Value &, Json::Value &);
		void onParticipantEvicted(const Json::Value &, Json::Value &);
		void onNewMessage(const Json::Value &, Json::Value &);

	protected:
		bool isRoomAvailable();

	private:
		std::shared_ptr<kurento::JsonRpc::Handler> _handler;
		std::shared_ptr<WebSocketTransport> _transport;
		std::shared_ptr<kurento::JsonRpc::Client> _client;

		std::string _wsUri;

		QSharedPointer<QWebRTCPeerConnectionFactory> _factory;
// 		QSharedPointer<QWebRTCPeerConnection> _peerSend;
// 		QSharedPointer<QWebRTCPeerConnection> _peerRecv;

		Room* _room = nullptr;
	};

	using KurentoPtr = std::shared_ptr<KurentoRoomClient>;
}
