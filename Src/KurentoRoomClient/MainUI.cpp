#include "stdafx.h"
#include "MainUI.h"
#include "FlowLayout.h"
#include "PeerUI.h"

#include "Room.h"
#include "Stream.h"
#include "Participant.h"

MainUI::MainUI(kurento::Room* room, kurento::KurentoPtr kurento, QWidget *parent)
	: QWidget(parent)
//	, kurento(kurento)
	, room(room)
{
	ui.setupUi(this);

	this->setAttribute(Qt::WA_DeleteOnClose);

	ui.line_room->setText(QString::fromStdString(room->roomName()));
	ui.line_user->setText(QString::fromStdString(room->userName()));

	FlowLayout *flowLayout = new FlowLayout;
	ui.widget_display->setLayout(flowLayout);

 	localStream = kurento->stream(room, true, true, false);
 
 	localStream->on("access-accepted", [=]()
 	{
 		this->steamAccessAccepted();
 	});
 
 	localStream->on("access-denied", [=]()
 	{
 		this->steamAccessDenied();
 	});
 
 	localStream->init();

	for (auto participant : room->getParticipant())
	{
		insertPeer(participant);
	}

	room->on("participant-joined", [=](kurento::Participant* participant)
	{
		insertPeer(participant);
	});

	room->on("participant-published", [=](kurento::Participant* participant)
	{
		updatePeer(participant);
	});

	room->on("participant-left", [=](kurento::Participant* participant)
	{
		removePeer(participant);
	});

	room->on("stream-added", [=](kurento::Stream* stream)
	{
		//QMetaObject::invokeMethod(this, "streamAdded", Q_ARG(kurento::Stream*, stream));
	});

	room->on("stream-subscribed", [=](kurento::Stream* stream)
	{
		streamSubscribed(stream);
	});
	
	room->on("participant-evicted", [=]()
	{

	});

	room->on("newMessage", [=](std::string user,std::string msg)
	{
		newMessage(user, msg);
	});

	connect(ui.btn_send, &QPushButton::clicked, this, &MainUI::doSendMessage);
	connect(ui.line_text, &QLineEdit::textChanged, this, [=]()
	{
		ui.btn_send->setEnabled(!ui.line_text->text().isEmpty());
	});
	
	room->init();
}

MainUI::~MainUI()
{
	
}

void MainUI::steamAccessAccepted()
{
	localStream->publish();
}

void MainUI::steamAccessDenied()
{

}

void MainUI::insertPeer(kurento::Participant* participant)
{
	PeerUI* peerUI = new PeerUI(this);
	peerUI->init(participant);

	peerUIs[participant->getID()] = peerUI;

	ui.widget_display->layout()->addWidget(peerUI);
}

void MainUI::updatePeer(kurento::Participant* participant)
{
	std::string pid = participant->getID();
	if (peerUIs.contains(pid))
	{
		PeerUI* peerUI = peerUIs[pid];
		peerUI->init(participant);
	}
}

void MainUI::removePeer(kurento::Participant* participant)
{
	std::string pid = participant->getID();

	if (peerUIs.contains(pid))
	{
		PeerUI* peerUI = peerUIs[pid];

		ui.widget_display->layout()->removeWidget(peerUI);
		peerUI->deleteLater();

		peerUIs.remove(pid);
	}
}

void MainUI::streamSubscribed(kurento::Stream* stream)
{
	kurento::Participant* participant = stream->getParticipant();
	std::string pid = participant->getID();

	if (peerUIs.contains(pid))
	{
		PeerUI* peerUI = peerUIs[pid];
		peerUI->streamSubscribed(stream);
	}
}

void MainUI::newMessage(std::string user, std::string msg)
{
	bool fromMe = user == room->userName();

	ui.text_dispaly->appendMessage(QString::fromStdString(msg), QString::fromStdString(user), fromMe);
}

void MainUI::doSendMessage()
{
	QString text = ui.line_text->text();
	std::string msg = text.toStdString();

	if (!text.isEmpty())
	{
		room->sendMessage(msg);
		ui.line_text->clear();
	}
}
