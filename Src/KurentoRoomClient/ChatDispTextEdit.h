#pragma once

#include <QTextBrowser>

class ChatDispTextEdit : public QTextBrowser
{
	Q_OBJECT

public:
	ChatDispTextEdit(QWidget *parent);
	~ChatDispTextEdit();

public:
	void appendMessage(QString msg, QString sender, bool fromMe = true);

protected slots:
	void slotFont(QString f);
	void slotSize(int num);
	void slotColor(QColor color);
	void slotAppedText(QString text);
	void slotAppedSpace(int n = 1);

	void mergeFormat(QTextCharFormat fmt);
};
