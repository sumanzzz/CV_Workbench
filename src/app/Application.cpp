#include "Application.h"
#include "MainWindow.h"

#include "ImageLoader.h"

#include <QMainWindow>
#include <QFileDialog>
#include <qDebug>

Application::Application(int& argc , char* argv[]) : m_app(argc , argv){}

int Application::run()
{
	
	MainWindow window;
	window.show();

	return m_app.exec();

	
}

