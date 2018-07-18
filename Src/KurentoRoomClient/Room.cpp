#include "stdafx.h"
#include "Room.h"
#include "Participant.h"
#include "Stream.h"

namespace kurento
{

	Room::Room(KurentoPtr kurento, const Options& options)
		: kurento(kurento)
		, options(options)
	{
		Json::Value op;
		op["id"] = options.user;

		localParticipant = new Participant(kurento, true, this, op);
		participants[options.user] = localParticipant;
	}

	Room::~Room()
	{
	}

	void Room::connect()
	{
		kurento->joinRoom(options.room, options.user, false, [=](const Json::Value &result, bool isError)
		{
			if (isError)
			{
				this->connected = false;

				this->emit("error_room", result);
			}
			else
			{
				std::string sessionId = result["sessionId"].asString();
				Json::Value jsonPeers = result["value"];
				for (const auto& peer : jsonPeers)
				{
					Participant* participant = new Participant(kurento, false, this, peer);

					participants[participant->getID()] = participant;

					auto streams = participant->getStreams();
					for (auto stream : streams) {
						//roomEvent.streams.push(streams[key]);
						if (options.subscribeToStreams) {
							stream->subscribe();
						}
					}
					
				}

				this->connected = true;

				this->emit("room_connected");
			}
		});
	}

	void Room::init()
	{
		if (inited)
		{
			return;
		}

// 		for (auto it : participants)
// 		{
// 			auto participant = it.second;
// 
// 			auto streams = participant->getStreams();
// 			for (auto stream : streams) {
// 				//roomEvent.streams.push(streams[key]);
// 				if (options.subscribeToStreams) {
// 					stream->subscribe();
// 				}
// 			}
// 		}		

		inited = true;
	}

	void Room::subscribe(Stream* stream)
	{

	}

	void Room::leave(bool forced/* = false*/)
	{
		if (connected && !forced) {
			kurento->leaveRoom([=](const Json::Value &result, bool isError)
			{
				if (isError) {
					//console.error(error);
				}
				//jsonRpcClient.close();
			});
		}
		else {
			//jsonRpcClient.close();
		}
		connected = false;
		if (participants.size()) 
		{
			for (auto pid : participants) 
			{
// 				participants[pid].dispose();
// 				delete participants[pid];
			}
		}
	}

	void Room::sendMessage(std::string message)
	{
		kurento->sendMessage(options.user, options.room, message, [=](const Json::Value &result, bool isError)
		{
			if (isError)
			{

			}
		});
	}

	bool Room::isconnected() const
	{
		return this->connected;
	}

	std::string Room::roomName() const
	{
		return options.room;
	}

	std::string Room::userName() const
	{
		return options.user;
	}

	Participant* Room::getLocalParticipant()
	{
		return localParticipant;
	}

	std::list<Participant*> Room::getParticipant()
	{
		std::list<Participant*> list;
		for (auto it : this->participants)
		{
			list.push_back(it.second);
		}
		return list;
	}

	std::map<std::string, Stream*>& Room::getStreams()
	{
		return this->streams;
	}

	bool Room::recvIceCandidate(const Json::Value& msg) {

		std::string endpointName = msg["endpointName"].asString();

		Participant* participant = participants[endpointName];
		if (!participant) {
// 			qCritical("Participant not found for endpoint " +
// 				endpointName + ". Ice candidate will be ignored.",
// 				candidate);
			return false;
		}
		std::list<Stream*> streams = participant->getStreams();
		for (auto stream : streams)
		{
			std::string sdpMid = msg["sdpMid"].asString();
			int sdpMLineIndex = msg["sdpMLineIndex"].asInt();
			std::string candidate = msg["candidate"].asString();

			stream->addIceCandidate(sdpMid, sdpMLineIndex, candidate);
		}

		return true;
	}

	void Room::onParticipantJoined(const Json::Value& msg)
	{
		Participant* participant = new Participant(kurento, false, this, msg);
		std::string pid = participant->getID();

		if (!participants.count(pid)) 
		{
			//console.log("New participant to participants list with id", pid);
			participants[pid] = participant;
		}
 		else {
 			//use existing so that we don't lose streams info
			/*console.info("Participant already exists in participants list with " +
				"the same id, old:", participants[pid], ", joined now:", participant);*/
 			participant = participants[pid];
 		}

		emit("participant-joined", participant);
	}

	void Room::onParticipantPublished(const Json::Value& msg)
	{
		Participant* participant = new Participant(kurento, false, this, msg);
		std::string pid = participant->getID();

		if (!participants.count(pid)) 
		{
			//console.info("Publisher not found in participants list by its id", pid);
		}
		else
		{
			delete participants[pid];
			participants.erase(pid);
			//console.log("Publisher found in participants list by its id", pid);
		}
		//replacing old participant (this one has streams)
		participants[pid] = participant;

		emit("participant-published", participant);

		auto streams = participant->getStreams();
		for (auto stream : streams)
		{
			if (options.subscribeToStreams) {
				stream->subscribe();
				emit("stream-added", stream);
			}
		}
	}

	void Room::onParticipantLeft(const Json::Value& msg)
	{
		std::string name = msg["name"].asString();
		if (participants.count(name))
		{
			Participant* participant = participants[name];
			participants.erase(name);

			emit("participant-left", participant);
		}
	}

	void Room::onParticipantEvicted(const Json::Value& msg)
	{
		emit("participant-evicted");
	}

	void Room::onNewMessage(const Json::Value& msg)
	{
		std::string user = msg["user"].asString();
		std::string message = msg["message"].asString();

		emit("newMessage", user, message);
	}

}
