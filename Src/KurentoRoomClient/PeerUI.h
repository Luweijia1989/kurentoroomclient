#pragma once

#include <QWidget>
#include "ui_PeerUI.h"
#include "KurentoRoomClient.h"

class QWebRTCVideo;
class QWebRTCMediaStream;

class PeerUI : public QWidget
{
	Q_OBJECT

public:
	PeerUI(QWidget *parent = Q_NULLPTR);
	~PeerUI();

	void init(kurento::Participant* peer);
	bool streamSubscribed(kurento::Stream* stream);

protected:
	void paintEvent(QPaintEvent *event);
	bool streamAdded(kurento::Stream* stream);

private slots:
	void frameChanged(int width, int height, const QByteArray& buffer, QImage::Format format = QImage::Format_ARGB32);

private:
	Ui::PeerUI ui;
	QLabel* label_name = nullptr;
	QWebRTCVideo* videoItem = nullptr;
	kurento::Participant* peer = nullptr;
};
