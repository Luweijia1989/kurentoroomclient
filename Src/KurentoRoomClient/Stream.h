#pragma once

#undef emit
#include <EventEmitter.hpp>
#include "KurentoRoomClient.h"

class QWebRTCMediaStream;

namespace kurento
{
	class Stream : public EventEmitter
	{
		friend class Room;
		friend class Participant;
		friend class KurentoRoomClient;

	protected:
		Stream(KurentoPtr kurento, bool local, Room* room, const Json::Value& options, Participant* peer);
		~Stream();

	public:
		std::string getID() const;
		std::string getGlobalID() const;
		Participant* getParticipant();

		void init();
		void publish();
		void subscribe();

		void addIceCandidate(std::string mId, int sdpMLineIndex, const std::string& sdpData);
		QSharedPointer<QWebRTCMediaStream> getStream();

		typedef std::function<void(bool result, std::string sdp) >
			CallBack;

	protected:
		void initWebRtcPeer(CallBack callback);
		void publishVideoCallback(bool result,std::string sdp);
		void startVideoCallback(bool result, std::string sdp);
		void processSdpAnswer(std::string sdp);
	private:
		KurentoPtr kurento;
		Room* room = nullptr;
		Participant* participant = nullptr;
		std::string id;
		bool local = false;
		Json::Value options;
		QSharedPointer<QWebRTCMediaStream> _stream;
		QSharedPointer<QWebRTCPeerConnection> _peer;
	};

}
