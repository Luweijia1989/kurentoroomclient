#pragma once

#include <QWidget>
#include "ui_MainUI.h"
#include "KurentoRoomClient.h"

class PeerUI;

class MainUI : public QWidget
{
	Q_OBJECT

public:
	MainUI(kurento::Room* room, kurento::KurentoPtr kurento, QWidget *parent = Q_NULLPTR);
	~MainUI();

protected:
	void steamAccessAccepted();
	void steamAccessDenied();

	void insertPeer(kurento::Participant* participant);
	void updatePeer(kurento::Participant* participant);
	void removePeer(kurento::Participant* participant);

private slots:
	void streamSubscribed(kurento::Stream* stream);
	void newMessage(std::string user, std::string msg);
	void doSendMessage();

private:
	Ui::MainUI ui;

	kurento::Room* room = nullptr;
	kurento::Stream* localStream = nullptr;
//	kurento::KurentoPtr kurento;

	QMap<std::string, PeerUI*> peerUIs;
};
