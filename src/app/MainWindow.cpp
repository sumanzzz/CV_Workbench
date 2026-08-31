#include "MainWindow.h"

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>

MainWindow::MainWindow(QWidget* parent) :QMainWindow(parent)
{
	setWindowTitle(AppConfig::WINDOW_TITLE);
	resize(AppConfig::WINDOW_LENGTH, AppConfig::WINDOW_WIDTH);

	QWidget* centralWindow = new QWidget(this);
	setCentralWidget(centralWindow);

	QGridLayout* layout = new QGridLayout(centralWindow);

	QLabel* label = new QLabel("Write Something", centralWindow);
	QLineEdit* input = new QLineEdit(centralWindow);
	QPushButton* button = new QPushButton("Click Me", centralWindow);

	layout->addWidget(label);
	layout->addWidget(input);
	layout->addWidget(button);

	QWidget::connect(
		button,
		&QPushButton::clicked,
		[label]()
		{
			label->setText("button said click");
		}
	);

}
	

