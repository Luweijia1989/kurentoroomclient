#include "stdafx.h"
#include "WebSocketTransport.h"



WebSocketTransport::WebSocketTransport(QObject *parent)
	: QObject(parent)
{
	connect(&m_webSocket, &QWebSocket::connected, this, &WebSocketTransport::onConnected);
	connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &WebSocketTransport::onTextMessageReceived);
	connect(&m_webSocket, &QWebSocket::disconnected, this, &WebSocketTransport::onDisConnected);

	typedef void (QWebSocket:: *sslErrorsSignal)(const QList<QSslError> &);
	connect(&m_webSocket, static_cast<sslErrorsSignal>(&QWebSocket::sslErrors), this, &WebSocketTransport::onSslErrors);

	typedef void (QWebSocket:: *errorsSignal)(QAbstractSocket::SocketError);
	connect(&m_webSocket, static_cast<errorsSignal>(&QWebSocket::error), this, &WebSocketTransport::onErrors);

	connect(&m_webSocket, &QWebSocket::aboutToClose, this, [=]() mutable
	{
		qWarning() << "aboutToClose";
	});
}

WebSocketTransport::~WebSocketTransport()
{

}

void WebSocketTransport::sendMessage(const std::string &data)
{
	QString output = QString::fromStdString(data);
	qWarning() << "WebSocket send to server: " << output.toStdString().c_str();

	m_webSocket.sendTextMessage(output);
}

void WebSocketTransport::doconnect(const std::string& url)
{
	m_url = QString::fromStdString(url);

	if (m_webSocket.isValid())
	{
		qWarning() << "m_webSocket::isValid";
	}

	qWarning() << "WebSocket connect to url: " << m_url;


	QNetworkRequest request(m_url);
	//request.setRawHeader("Sec-WebSocket-Protocol", "protoo");

	m_webSocket.open(request);
}

void WebSocketTransport::close()
{
	m_webSocket.close();
}

void WebSocketTransport::onConnected()
{
	qWarning() << "WebSocket connected";

	emit("onconnected");
}

void WebSocketTransport::onTextMessageReceived(QString message)
{
	qWarning() << "WebSocket recv from server: " << message.toStdString().c_str();

	this->messageReceived(message.toStdString());
}

void WebSocketTransport::onSslErrors(const QList<QSslError> &errors)
{
	QStringList strErrors;

	for (auto& error : errors)
	{
		strErrors += error.errorString();
	}

	m_webSocket.ignoreSslErrors();
}

void WebSocketTransport::onErrors(QAbstractSocket::SocketError error)
{
	qWarning() << "WebSocketTransport::onErrors: " << error;

	emit("onconnected");
}

void WebSocketTransport::onDisConnected()
{
	qWarning() << "WebSocket disconnected";

	emit("onconnected");
}

void WebSocketTransport::onRequestTimeout(uint32_t id)
{
	
}
