#pragma once

#undef emit
#include <EventEmitter.hpp>
#include "KurentoRoomClient.h"

class QWebRTCIceCandidate;

namespace kurento
{
	class Participant
	{
		friend class Room;
		friend class Stream;

	protected:
		Participant(KurentoPtr kurento, bool local, Room* room, const Json::Value& options);
		~Participant();

	public:
		std::string getID() const;
		bool islocal() const;
		std::list<Stream*> getStreams();
		void addStream(Stream* stream);

		void sendIceCandidate(const QSharedPointer<QWebRTCIceCandidate>&);

	private:
		std::string id;
		bool local;
		Room* room = nullptr;
		std::map<std::string, Stream*> streams;
		KurentoPtr kurento;
	};

}
