#include "stdafx.h"
#include "LoginUI.h"
#include "ProxyStyle.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	//加载qss文件
	QFile qssfile(":/qss/Resources/main.qss");
	if (qssfile.open(QFile::ReadOnly))
	{
		QString styleSheet = QString(qssfile.readAll());
		qssfile.close();
		a.setStyleSheet(styleSheet);
	}

	a.setStyle(new cProxyStyle);//去除焦点虚框

	LoginUI *w = new LoginUI;
	w->show();
	return a.exec();
}
