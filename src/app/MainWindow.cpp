#include "MainWindow.h"
#include "ImageLoader.h"

#include <opencv2/core.hpp>

#include <QWidget>

#include <QGridLayout>
#include <QVBoxLayout>

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include <QSlider>
#include <QMenuBar>
#include <QImage>
#include <QPixmap>

#include <QSizePolicy>
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

	// IMAGE AREA
	imageDisplay = new QLabel(centralWindow);
	imageDisplay->setAlignment(Qt::AlignCenter);
	imageDisplay->setText(ALERT::NO_IMAGE);
	imageDisplay->setScaledContents(false);

	layout->addWidget(imageDisplay);

	// TOOLS PANEL
	toolsPanel = new QWidget(centralWindow);
	layout->addWidget(toolsPanel, 0, 2, 3, 1);
	toolsLayout = new QVBoxLayout(toolsPanel);

	QLabel* toolsTitle = new QLabel(AppConfig::TOOLS_PANEL_TITLE, toolsPanel);
	toolsLayout->addWidget(toolsTitle);
	toolsLayout->setAlignment(Qt::AlignTop);
	toolsLayout->setAlignment(toolsTitle, Qt::AlignHCenter);

	//BLUR
	
	


	// SIGNALS AND SLOTS
	QWidget::connect(
		fileAction,
		&QAction::triggered,
		this,
		&MainWindow::openFile
	);

	QWidget::connect(
		toolAction,
		&QAction::triggered,
		this,
		&MainWindow::showBlurTools
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
		cv::Mat loadedImage = loader.load(filename.toStdString());
		
		if (loadedImage.empty())
		{
			return;
		}
		m_image = new Image(loadedImage);

		QImage qImage(
			loadedImage.data,
			loadedImage.cols,
			loadedImage.rows,
			static_cast<int>(loadedImage.step),
			QImage::Format_RGB888
		);
		imageDisplay->setPixmap(QPixmap::fromImage(qImage.copy()));
		
	}
}

void MainWindow::showBlurTools()
{
	
	QLabel* blurTitle = new QLabel(AppConfig::BLUR, toolsPanel);
	toolsLayout->addWidget(blurTitle);

	// BLUR TYPE COMBOBOX
	QComboBox* blurType = new QComboBox(toolsPanel);
	blurType->addItem(AppConfig::GAUSSIAN_BLUR);
	blurType->addItem(AppConfig::MEDIAN_BLUR);
	blurType->addItem(AppConfig::BOX_BLUR);
	toolsLayout->addWidget(blurType);

	// SLIDER FOR  BLUR
	QSlider* slider = new QSlider(Qt::Horizontal, toolsPanel);
	slider->setMinimum(1);
	slider->setMaximum(10);
	slider->setValue(1);
	toolsLayout->addWidget(slider);

	QLabel* value = new QLabel("Kernel : 1", toolsPanel);
	toolsLayout->addWidget(value);

	QObject::connect(
		slider,
		&QSlider::valueChanged,
		this,
		[this, value  ,blurType](int val)
		{
			int kernelSize = (2 * val + 1);
			value->setText("Kernel: " + QString::number(kernelSize));

			cv::Mat blurred;
			switch (blurType->currentIndex())
			{
			case 0: blurred = m_image->gaussianBlur(kernelSize); break;
			case 1:   blurred = m_image->medianBlur(kernelSize); break;
			case 2:      blurred = m_image->boxBlur(kernelSize); break;
			}
			qDebug() << blurType<< " Blur :" << kernelSize;

			QImage qImage(
				blurred.data,
				blurred.cols,
				blurred.rows,
				static_cast<int>(blurred.step),
				QImage::Format_RGB888
			);
			QPixmap pixmap = QPixmap::fromImage(qImage);

			imageDisplay->setPixmap(
				pixmap.scaled(
					imageDisplay->size(),
					Qt::KeepAspectRatio,
					Qt::SmoothTransformation
				)
			);
		}
	);

}	