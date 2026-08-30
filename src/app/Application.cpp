#include "Application.h"

#include <QMainWindow>

constexpr int WINDOW_LENGTH = 800;
constexpr int WINDOW_WIDTH = 600;
constexpr auto WINDOW_TITLE = "Computer Vision Workbench";

Application::Application(int& argc , char* argv[]) : m_app(argc , argv){}

int Application::run()
{
	QMainWindow window;

	window.setWindowTitle(WINDOW_TITLE);
	window.resize(WINDOW_LENGTH, WINDOW_WIDTH);

	window.show();

	return m_app.exec();
}

