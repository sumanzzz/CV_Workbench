#pragma once

#include <QApplication>

class Application
{
private:
	QApplication m_app;

public:
	Application(int& argc, char* argv[]);

	int run();


};