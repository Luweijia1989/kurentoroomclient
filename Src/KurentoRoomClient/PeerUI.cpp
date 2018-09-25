#include "stdafx.h"
#include "PeerUI.h"
#include "Participant.h"
#include "Stream.h"
#include <qwebrtcmediatrack.hpp>
#include <qwebrtcmediastream.hpp>
#include "qwebrtcvideo.hpp"

PeerUI::PeerUI(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	setFixedSize(270, 180);

	ui.label_render->setAlignment(Qt::AlignCenter);
	label_name = new QLabel(this);
	label_name->setObjectName("label_name");
	label_name->setContentsMargins(10, 0, 10, 0);
	label_name->setFixedHeight(20);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(label_name, 0, Qt::AlignLeft);
	layout->addStretch();
	ui.label_render->setLayout(layout);

	QGraphicsDropShadowEffect *shadow_effect = new QGraphicsDropShadowEffect(this);
	shadow_effect->setOffset(5, 5);
	shadow_effect->setColor(Qt::gray);
	shadow_effect->setBlurRadius(9);
	ui.label_render->setGraphicsEffect(shadow_effect);
}

PeerUI::~PeerUI()
{

}

void PeerUI::init(kurento::Participant* peer)
{
	this->peer = peer;

	label_name->setText(QString::fromStdString(peer->getID()));

	for (auto stream : peer->getStreams())
	{
		if (streamAdded(stream))
		{
			break;
		}
	}
}

bool PeerUI::streamSubscribed(kurento::Stream* stream)
{
	return streamAdded(stream);
}

bool PeerUI::streamAdded(kurento::Stream* stream)
{
	if (!stream)
	{
		return false;
	}

	QSharedPointer<QWebRTCMediaStream> wstream = stream->getStream();
	if (wstream.isNull())
	{
		return false;
	}

	for (QSharedPointer<QWebRTCMediaTrack> track : wstream->tracks())
	{
		if (track->trackType() == QWebRTCMediaTrack::Type::Video)
		{
			videoItem = new QWebRTCVideo();
			auto thread = qApp->thread();
			videoItem->moveToThread(thread);
			videoItem->setVideoTrack(track.data());
			connect(videoItem, &QWebRTCVideo::frameChanged, this, &PeerUI::frameChanged);
			break;
		}
	}
	
	return true;
}

void PeerUI::paintEvent(QPaintEvent *event)
{
	QStyleOption opt;
	opt.init(this);
	QPainter p(this);
	style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void PeerUI::frameChanged(int width, int height, const QByteArray& buffer, QImage::Format format/* = QImage::Format_ARGB32*/)
{
	QImage image((uchar *)buffer.data(), width, height, format);

	ui.label_render->setPixmap(QPixmap::fromImage(image));
}
