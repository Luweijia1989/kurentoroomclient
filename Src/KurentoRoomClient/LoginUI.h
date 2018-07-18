#pragma once

#include <QWidget>
#include "ui_LoginUI.h"

#include "KurentoRoomClient.h"

class BusyIndicator;

class LoginUI : public QWidget
{
	Q_OBJECT

public:
	LoginUI(QWidget *parent = Q_NULLPTR);
	~LoginUI();

protected slots:
	void btnJoinClicked();

protected:
	virtual void closeEvent(QCloseEvent *event);

private:
	Ui::LoginUI ui;
	BusyIndicator* busy = nullptr;

	kurento::Room* room = nullptr;
	kurento::KurentoPtr kurento;
};
