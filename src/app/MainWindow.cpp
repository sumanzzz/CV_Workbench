#include "MainWindow.h"
#include "ImageLoader.h"
#include <opencv2/core.hpp>

#include <QWidget>

#include <QGridLayout>

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMenuBar>
#include <QLabel>
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

	// WORKSPACE
	imageDisplay = new QLabel(centralWindow);

	imageDisplay->setAlignment(Qt::AlignCenter);
	imageDisplay->setText("No Image Loaded");

	layout->addWidget(imageDisplay);


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
