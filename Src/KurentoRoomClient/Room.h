#pragma once

#undef emit
#include <EventEmitter.hpp>
#include "KurentoRoomClient.h"

namespace kurento
{
	class Participant;

	class Room : public EventEmitter
	{
		friend class KurentoRoomClient;

	protected:
		Room(KurentoPtr kurento, const Options& options);
		~Room();

	public:
		void connect();
		void init();
		void subscribe(Stream* stream);
		void leave(bool forced = false);
		void sendMessage(std::string message);

		bool isconnected() const;
		std::string roomName() const;
		std::string userName() const;
		Participant* getLocalParticipant();
		std::list<Participant*> getParticipant();

		std::map<std::string, Stream*>& getStreams();

		bool recvIceCandidate(const Json::Value& msg);
		void onParticipantJoined(const Json::Value& msg);
		void onParticipantPublished(const Json::Value& msg);
		void onParticipantLeft(const Json::Value& msg);
		void onParticipantEvicted(const Json::Value& msg);
		void onNewMessage(const Json::Value& msg);


	private:
		KurentoPtr kurento;
		bool connected = false;
		bool inited = false;
		Options options;
		std::map<std::string, Stream*> streams;
		std::map<std::string, Participant*> participants;

		Participant* localParticipant = nullptr;
	};
}

