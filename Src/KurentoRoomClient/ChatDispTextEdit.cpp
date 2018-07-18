#include "stdafx.h"
#include "ChatDispTextEdit.h"

ChatDispTextEdit::ChatDispTextEdit(QWidget *parent)
	: QTextBrowser(parent)
{
	setReadOnly(true);
	setOpenExternalLinks(false);
	setOpenLinks(false);

	QPalette p;
	p.setColor(QPalette::Inactive, QPalette::HighlightedText, QColor(255, 255, 255));
	p.setColor(QPalette::Inactive, QPalette::Highlight, QColor(49, 106, 197));
	setPalette(p);

}

ChatDispTextEdit::~ChatDispTextEdit()
{
}

void ChatDispTextEdit::slotFont(QString f)
{
	QTextCharFormat fmt;
	fmt.setFontFamily(f + ",Segoe UI Emoji");

	mergeFormat(fmt);
}

void ChatDispTextEdit::slotSize(int num)
{
	QTextCharFormat fmt;
	QTextCursor cursor = textCursor();
	QFont fFont = cursor.charFormat().font();
	fFont.setPixelSize(/*fontPx2SuitablePx(*/num);
	fmt.setFont(fFont);

	mergeFormat(fmt);

}

void ChatDispTextEdit::slotColor(QColor color)
{
	if (color.isValid())
	{
		QTextCharFormat fmt;
		fmt.setForeground(color);
		mergeFormat(fmt);
	}
}

void ChatDispTextEdit::slotAppedText(QString text)
{
	QTextCursor cursor = textCursor();
	QTextCharFormat tcf = cursor.charFormat();
	QTextCharFormat tcf2;
	tcf2.setFont(tcf.font());
	tcf2.setForeground(tcf.foreground());
	cursor.insertText(text, tcf2);
}

void ChatDispTextEdit::mergeFormat(QTextCharFormat fmt)
{
	QTextCursor cursor = textCursor();

	//       if ( !cursor.hasSelection() )
	//              cursor.select( QTextCursor::WordUnderCursor );
	cursor.mergeCharFormat(fmt);
	mergeCurrentCharFormat(fmt);
}

void ChatDispTextEdit::slotAppedSpace(int n)
{
	moveCursor(QTextCursor::End);
	QTextCharFormat tcf1;

	QString str(n, QChar(' '));
	textCursor().insertText(str, tcf1);
}

void ChatDispTextEdit::appendMessage(QString msg, QString sender, bool fromMe/* = true*/)
{
	moveCursor(QTextCursor::End);
	QTextBlockFormat fmt;
	fmt.setLeftMargin(10);
	fmt.setTopMargin(6);
	fmt.setBottomMargin(0);
	textCursor().mergeBlockFormat(fmt);

	//显示昵称和时间
	QString head;
	head += sender;
	head += QString(" ");
	head += QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss:zzz");
	slotColor(fromMe ? QColor("#0C87B1") : QColor("#05A74E"));

	//slotFont("Microsoft YaHei");
	//slotSize(QString::number(fontPx2SuitablePx(10)));
	slotAppedText(head);

	fmt.setTopMargin(0);
	QTextCharFormat tcf;
	textCursor().insertBlock(fmt, tcf);

	//光标移植末尾并插入2个空格
	slotAppedSpace(2);
	slotFont(QString::fromStdWString(L"Microsoft YaHei"));
	slotSize(14);
	slotAppedText(msg);

	//slotAppedText(QString::fromStdWString(L"11alskdflaskdjlfajsldkfjasldjflasj"));

	QTextBlockFormat fmt1;
	QTextCharFormat tcf2;
	textCursor().insertBlock(fmt1, tcf2);

	moveCursor(QTextCursor::End);
}
