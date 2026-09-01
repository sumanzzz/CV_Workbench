#include "MainWindow.h"
#include "ImageLoader.h"
#include <opencv2/core.hpp>

#include <QWidget>

#include <QGridLayout>
#include <QVBoxLayout>

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMenuBar>
#include <QImage>
#include <QPixmap>

#include <QString>
#include <QDebug>
#include <QFileDialog>

MainWindow::MainWindow(QWidget* parent) :QMainWindow(parent)
{
	setWindowTitle(AppConfig::WINDOW_TITLE);
	resize(AppConfig::WINDOW_LENGTH, AppConfig::WINDOW_WIDTH);

	QWidget* centralWindow = new QWidget(this);
	setCentralWidget(centralWindow);

	QGridLayout* layout = new QGridLayout(centralWindow);

	
	
	// MENU BAR
	QMenu* fileMenu = menuBar()->addMenu(AppConfig::MENU_FILE);
	QAction* fileAction = fileMenu->addAction(AppConfig::FILE_OPEN);

	QMenu* toolsMenu = menuBar()->addMenu(AppConfig::MENU_TOOL);
	QAction* toolAction = toolsMenu->addAction(AppConfig::TOOL_BLUR);

	// WORKSPACE
	imageDisplay = new QLabel(centralWindow);

	imageDisplay->setAlignment(Qt::AlignCenter);
	imageDisplay->setText(ALERT::NO_IMAGE);

	layout->addWidget(imageDisplay);

	// TOOLS PANEL
	QWidget* toolsPanel = new QWidget(centralWindow);
	layout->addWidget(toolsPanel, 0, 2, 3, 1);

	QVBoxLayout* toolsLayout = new QVBoxLayout(toolsPanel);

	QLabel* toolsTitle = new QLabel(AppConfig::TOOLS_PANEL_TITLE, toolsPanel);
	toolsLayout->addWidget(toolsTitle);
	toolsLayout->setAlignment(Qt::AlignTop);
	toolsLayout->setAlignment(toolsTitle, Qt::AlignHCenter);


	// SIGNALS AND SLOTS
	QWidget::connect(
		fileAction,
		&QAction::triggered,
		this,
		&MainWindow::openFile
	);

}

void MainWindow::openFile()
{
	QString filename = QFileDialog::getOpenFileName(
		this,
		"Open File",
		"",
		"Images(*.png *.jpg *.jpeg *.bmp)"
	);
	if (!filename.isEmpty())
	{
		ImageLoader loader;
		cv::Mat image = loader.load(filename.toStdString());
		
		if (image.empty())
		{
			return;
		}

		QImage qImage(
			image.data,
			image.cols,
			image.rows,
			static_cast<int>(image.step),
			QImage::Format_RGB888
		);
		imageDisplay->setPixmap(QPixmap::fromImage(qImage.copy()));
	}
}
