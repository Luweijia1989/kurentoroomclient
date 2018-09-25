#pragma once

#include <memory>
#include <QtNetwork>
#include <QWebSocket>
#include <JsonRpcClient.hpp>
#undef emit
#include <EventEmitter.hpp>

class WebSocketTransport : public QObject, public kurento::JsonRpc::Transport , public EventEmitter
{
	Q_OBJECT

public:
	WebSocketTransport(QObject *parent = nullptr);
	~WebSocketTransport();

public:
	virtual void sendMessage(const std::string &data);

public slots:
	void doconnect(const std::string& url);
	void close();

protected slots:
	void onConnected();
	void onTextMessageReceived(QString message);
	void onSslErrors(const QList<QSslError> &errors);
	void onErrors(QAbstractSocket::SocketError error);
	void onDisConnected();

	void onRequestTimeout(uint32_t id);

private:


private:
	QWebSocket m_webSocket;
	QUrl m_url;
};
