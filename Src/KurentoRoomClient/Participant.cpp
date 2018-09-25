#include "stdafx.h"
#include "Participant.h"
#include "Stream.h"
#include "Room.h"
#include <qwebrtcicecandidate.hpp>

namespace kurento
{
	Participant::Participant(KurentoPtr kurento, bool local, Room* room, const Json::Value& options)
		: kurento(kurento)
		, room(room)
		, local(local)
	{
		id = options["id"].asString();

		const Json::Value& streams = options["streams"];

		Json::Value streamsOpts;

		for (const auto& stream : streams)
		{
			std::string streamId = stream["id"].asString();

			Json::Value streamOpts;
			streamOpts["id"] = stream["id"];
			streamOpts["recvVideo"] = stream["recvVideo"].asBool();
			streamOpts["recvAudio"] = stream["recvAudio"].asBool();

			Stream* stream = new Stream(kurento, false, room, streamOpts, this);
			addStream(stream);
			streamsOpts.append(streamOpts);
		}
	}

	Participant::~Participant()
	{
	}

	std::string Participant::getID() const
	{
		return this->id;
	}

	bool Participant::islocal() const
	{
		return this->local;
	}

	std::list<Stream*> Participant::getStreams()
	{
		std::list<Stream*> liststreams;
		for (auto it : streams)
		{
			liststreams.push_back(it.second);
		}

		return liststreams;
	}

	void Participant::addStream(Stream* stream)
	{
		streams[stream->getID()] = stream;
		room->getStreams()[stream->getID()] = stream;
	}

	void Participant::sendIceCandidate(const QSharedPointer<QWebRTCIceCandidate>& candidate)
	{
		kurento->onIceCandidate(getID(), candidate->sdp().toStdString(), candidate->sdpMediaId().toStdString(),
			candidate->sdpMLineIndex(), [=](const Json::Value &result, bool isError)
		{
			if (isError)
			{
				
			}
			else
			{
				
			}
		});
	}
}
