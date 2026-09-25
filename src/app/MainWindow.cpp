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
#include <QCheckBox>
#include <QMouseEvent>

#include <QSizePolicy>
#include <QString>
#include <QDebug>
#include <QFileDialog>

#include <limits>

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
	imageDisplay->setFixedSize(AppConfig::IMAGE_MIN_W, AppConfig::IMAGE_MIN_H);
	imageDisplay->setScaledContents(false);
	imageDisplay->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	imageDisplay->setStyleSheet(
		"QLabel{"
		"border:1px solid #555;"
		"}"
	);
	imageDisplay->installEventFilter(this);
	layout->addWidget(imageDisplay);

	// TOOLS PANEL
	toolsPanel = new QWidget(centralWindow);
	layout->addWidget(toolsPanel, 0, 2, 3, 1);
	toolsLayout = new QVBoxLayout(toolsPanel);

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
		m_keyPoints.clear();
		m_selectedKeyPointIndex = -1;
		displayImage(loadedImage);
	}
}

// DISPLAY IMAGE
void MainWindow::displayImage(const cv::Mat& image)
{
	
	QImage qImage(
		image.data,
		image.cols,
		image.rows,
		static_cast<int>(image.step),
		QImage::Format_RGB888
	);
	QPixmap pixmap = QPixmap::fromImage(qImage);
	const QRect contentRect = imageDisplay->contentsRect();
	const QPixmap scaledPixmap = pixmap.scaled(
		contentRect.size(),
		Qt::KeepAspectRatio,
		Qt::SmoothTransformation
	);
	const QPoint topLeft(
		contentRect.x() + (contentRect.width() - scaledPixmap.width()) / 2,
		contentRect.y() + (contentRect.height() - scaledPixmap.height()) / 2
	);
	m_displayRect = QRect(topLeft, scaledPixmap.size());
	imageDisplay->setPixmap(scaledPixmap);
}

// KEY POINTS
void MainWindow::showKeyPoints()
{
	if (!m_image)
	{
		return;
	}

	m_keyPoints = m_image->detectKeypoints();
	m_selectedKeyPointIndex = -1;
	displayKeyPoints();
}

void MainWindow::displayKeyPoints()
{
	if (!m_image)
	{
		return;
	}

	std::vector<cv::KeyPoint> otherKeyPoints;
	otherKeyPoints.reserve(m_keyPoints.size());
	for (std::size_t index = 0; index < m_keyPoints.size(); ++index)
	{
		if (static_cast<int>(index) != m_selectedKeyPointIndex)
		{
			otherKeyPoints.push_back(m_keyPoints[index]);
		}
	}

	cv::Mat keyPointImage = m_image->drawKeyPoints(otherKeyPoints);
	if (m_selectedKeyPointIndex >= 0 &&
		m_selectedKeyPointIndex < static_cast<int>(m_keyPoints.size()))
	{
		std::vector<cv::KeyPoint> selectedKeyPoint{
			m_keyPoints[static_cast<std::size_t>(m_selectedKeyPointIndex)]
		};
		cv::Mat selectedKeyPointImage;
		cv::drawKeypoints(
			keyPointImage,
			selectedKeyPoint,
			selectedKeyPointImage,
			cv::Scalar(0, 0, 255),
			cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS
		);
		keyPointImage = selectedKeyPointImage;
	}

	displayImage(keyPointImage);
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == imageDisplay && event->type() == QEvent::MouseButtonPress)
	{
		auto* mouseEvent = static_cast<QMouseEvent*>(event);
		if (mouseEvent->button() == Qt::LeftButton &&
			m_image && !m_keyPoints.empty())
		{
			const QPoint displayPoint = mouseEvent->pos();
			if (m_displayRect.contains(displayPoint))
			{
				const double scaleX = static_cast<double>(m_image->getImage().cols) /
					m_displayRect.width();
				const double scaleY = static_cast<double>(m_image->getImage().rows) /
					m_displayRect.height();
				const cv::Point2f sourcePoint(
					static_cast<float>((displayPoint.x() - m_displayRect.x()) * scaleX),
					static_cast<float>((displayPoint.y() - m_displayRect.y()) * scaleY)
				);

				double nearestDistanceSquared = std::numeric_limits<double>::max();
				for (std::size_t index = 0; index < m_keyPoints.size(); ++index)
				{
					const cv::Point2f offset = m_keyPoints[index].pt - sourcePoint;
					const double distanceSquared = offset.dot(offset);
					if (distanceSquared < nearestDistanceSquared)
					{
						nearestDistanceSquared = distanceSquared;
						m_selectedKeyPointIndex = static_cast<int>(index);
					}
				}

				displayKeyPoints();
			}
		}
	}

	return QMainWindow::eventFilter(watched, event);
}

// BLUR
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

	QCheckBox* keyPointCheck = new QCheckBox("Show Keypoints", toolsPanel);
	toolsLayout->addWidget(keyPointCheck);

	QObject::connect(
		slider,
		&QSlider::valueChanged,
		this,
		[this, value  ,blurType](int val)
		{
			int kernelSize = (2 * val + 1);
			value->setText("Kernel: " + QString::number(kernelSize));

			cv::Mat blurred;
			blurred = m_image->applyBlur(static_cast<BlurType>(blurType->currentIndex()) , kernelSize);

			qDebug() << blurType<< " Blur :" << kernelSize;

			displayImage(blurred);
		}
	);
	QObject::connect(
		keyPointCheck,
		&QCheckBox::toggled,
		this,
		[this](bool checked)
		{
			if (checked)
			{
				showKeyPoints();
			}
			else
			{
				m_keyPoints.clear();
				m_selectedKeyPointIndex = -1;
				if (m_image)
				{
					displayImage(m_image->getImage());
				}

			}
			
		}
	);

}
