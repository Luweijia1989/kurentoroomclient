#include "stdafx.h"
#include "LoginUI.h"
#include "MainUI.h"
#include "BusyIndicator.h"
#include "Room.h"
#include "Stream.h"

LoginUI::LoginUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setAttribute(Qt::WA_DeleteOnClose);

	QString url = ui.line_server->text();

	kurento = std::make_shared<kurento::KurentoRoomClient>(url.toStdString());

	connect(ui.btnLogin, &QPushButton::clicked, this, &LoginUI::btnJoinClicked);
}

LoginUI::~LoginUI()
{
	kurento.reset();
}

void LoginUI::btnJoinClicked()
{
	QString userName = ui.line_user->text();
	if (userName.isEmpty())
	{
		ui.line_user->setFocus(Qt::MouseFocusReason);
		return;
	}

	QString roomName = ui.line_room->text();
	if (roomName.isEmpty())
	{
		ui.line_room->setFocus(Qt::MouseFocusReason);
		return;
	}

	kurento::Options options;
	options.room = roomName.toStdString();
	options.user = userName.toStdString();

	busy = new BusyIndicator(this);
	ui.btnLogin->setEnabled(false);

	room = kurento->room(options);

	room->on("room_connected", [=]()
	{
		auto mwPtr = std::make_unique<MainUI>(room, kurento);

		mwPtr->show();

		mwPtr.release();

		this->close();
	});

	room->on("error_room", [=](const Json::Value &result)
	{
		if (busy)
		{
			busy->deleteLater();
			busy = nullptr;
		}

		ui.btnLogin->setEnabled(true);

		room = nullptr;
	});

	room->connect();
}

void LoginUI::closeEvent(QCloseEvent *event)
{
	if (!(room && room->isconnected()))
	{
		qApp->quit();
	}
}
