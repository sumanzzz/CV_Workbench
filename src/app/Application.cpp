#include "Application.h"
#include "MainWindow.h"

#include "ImageLoader.h"

#include <QMainWindow>
#include <QFileDialog>
#include <QFile>
#include <qDebug>

Application::Application(int& argc , char* argv[]) : m_app(argc , argv){}

int Application::run()
{
	QFile stylesheetFile(":/styles/cv_workbench.qss");
	if (stylesheetFile.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		m_app.setStyleSheet(QString::fromUtf8(stylesheetFile.readAll()));
	}

	MainWindow window;
	window.show();

	return m_app.exec();
}

