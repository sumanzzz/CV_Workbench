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
#include <QPainter>
#include <QPen>
#include <QSignalBlocker>

#include <QSizePolicy>
#include <QString>
#include <QDebug>
#include <QFileDialog>

#include <limits>
#include <cmath>

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
	QAction* thresholdAction = toolsMenu->addAction(AppConfig::TOOL_THRESHOLD);

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

	QWidget::connect(
		thresholdAction,
		&QAction::triggered,
		this,
		&MainWindow::showThresholdTools
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
		m_processedImage.release();
		m_keyPoints.clear();
		m_selectedKeyPointIndex = -1;
		m_hasBlurPreview = false;
		m_keyPointsDetected = false;
		m_showKeyPoints = false;
		if (m_keyPointCheckBox)
		{
			const QSignalBlocker blocker(m_keyPointCheckBox);
			m_keyPointCheckBox->setChecked(false);
		}
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

	const cv::Mat& currentImage = m_processedImage.empty()
		? m_image->getImage()
		: m_processedImage;
	cv::Point2f previousSelection;
	const bool hadSelection = m_selectedKeyPointIndex >= 0 &&
		m_selectedKeyPointIndex < static_cast<int>(m_keyPoints.size());
	if (hadSelection)
	{
		previousSelection = m_keyPoints[static_cast<std::size_t>(m_selectedKeyPointIndex)].pt;
	}

	m_keyPoints = m_image->detectKeypoints(currentImage);
	m_keyPointsDetected = true;
	m_selectedKeyPointIndex = -1;
	if (hadSelection && !m_keyPoints.empty())
	{
		double nearestDistanceSquared = std::numeric_limits<double>::max();
		for (std::size_t index = 0; index < m_keyPoints.size(); ++index)
		{
			const cv::Point2f offset = m_keyPoints[index].pt - previousSelection;
			const double distanceSquared = offset.dot(offset);
			if (distanceSquared < nearestDistanceSquared)
			{
				nearestDistanceSquared = distanceSquared;
				m_selectedKeyPointIndex = static_cast<int>(index);
			}
		}
	}
	m_showKeyPoints = true;
	displayCurrentImage();
}

void MainWindow::displayCurrentImage()
{
	if (!m_image)
	{
		return;
	}

	const cv::Mat& currentImage = m_processedImage.empty()
		? m_image->getImage()
		: m_processedImage;
	if (m_showKeyPoints && m_keyPointsDetected)
	{
		displayKeyPoints(currentImage);
	}
	else
	{
		displayImage(currentImage);
	}
}

void MainWindow::displayKeyPoints(const cv::Mat& baseImage)
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

	cv::Mat keyPointImage = m_image->drawKeyPoints(baseImage, otherKeyPoints);
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
	if (m_selectedKeyPointIndex >= 0 &&
		m_selectedKeyPointIndex < static_cast<int>(m_keyPoints.size()))
	{
		const cv::Rect sourceRegion = m_image->keyPointRegion(
			m_keyPoints[static_cast<std::size_t>(m_selectedKeyPointIndex)]
		);
		if (!sourceRegion.empty())
		{
			const QRect displayRegion = mapSourceRectToDisplay(sourceRegion);
			QPixmap displayPixmap = imageDisplay->pixmap(Qt::ReturnByValue);
			QPainter painter(&displayPixmap);
			painter.setPen(QPen(Qt::yellow, 2));
			painter.drawRect(displayRegion.translated(-m_displayRect.topLeft()));
			painter.end();
			imageDisplay->setPixmap(displayPixmap);
		}
	}
}

QRect MainWindow::mapSourceRectToDisplay(const cv::Rect& sourceRect) const
{
	if (!m_image || sourceRect.empty())
	{
		return {};
	}

	const cv::Mat& sourceImage = m_image->getImage();
	const double scaleX = static_cast<double>(m_displayRect.width()) / sourceImage.cols;
	const double scaleY = static_cast<double>(m_displayRect.height()) / sourceImage.rows;
	const int left = m_displayRect.x() + static_cast<int>(std::lround(sourceRect.x * scaleX));
	const int top = m_displayRect.y() + static_cast<int>(std::lround(sourceRect.y * scaleY));
	const int right = m_displayRect.x() + static_cast<int>(std::lround((sourceRect.x + sourceRect.width) * scaleX));
	const int bottom = m_displayRect.y() + static_cast<int>(std::lround((sourceRect.y + sourceRect.height) * scaleY));

	return QRect(left, top, right - left, bottom - top);
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
	if (watched == imageDisplay && event->type() == QEvent::MouseButtonPress)
	{
		auto* mouseEvent = static_cast<QMouseEvent*>(event);
		if (mouseEvent->button() == Qt::LeftButton &&
			m_image && m_showKeyPoints && !m_keyPoints.empty())
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

				const int previousSelectionIndex = m_selectedKeyPointIndex;
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

				if (m_selectedKeyPointIndex != previousSelectionIndex)
				{
					resetBlurSlider();
					displayCurrentImage();
				}
				else if (m_hasBlurPreview)
				{
					applyBlur();
				}
				else
				{
					displayCurrentImage();
				}
			}
		}
	}

	return QMainWindow::eventFilter(watched, event);
}

// BLUR
void MainWindow::showBlurTools()
{
	if (m_blurSlider)
	{
		return;
	}
	
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
	slider->setMinimum(0);
	slider->setMaximum(10);
	slider->setValue(0);
	toolsLayout->addWidget(slider);
	m_blurSlider = slider;

	QLabel* value = new QLabel("Kernel: 1", toolsPanel);
	toolsLayout->addWidget(value);
	m_kernelValueLabel = value;

	QCheckBox* keyPointCheck = new QCheckBox("Show Keypoints", toolsPanel);
	toolsLayout->addWidget(keyPointCheck);
	m_keyPointCheckBox = keyPointCheck;

	QPushButton* resetButton = new QPushButton("Reset Image", toolsPanel);
	toolsLayout->addWidget(resetButton);
	QObject::connect(resetButton, &QPushButton::clicked, this, &MainWindow::resetImage);

	QObject::connect(
		slider,
		&QSlider::valueChanged,
		this,
		[this, value, blurType](int val)
		{
			m_blurKernelSize = (2 * val + 1);
			m_blurType = static_cast<BlurType>(blurType->currentIndex());
			m_hasBlurPreview = true;
			value->setText("Kernel: " + QString::number(m_blurKernelSize));
			qDebug() << blurType << " Blur :" << m_blurKernelSize;
			applyBlur();
		}
	);
	QObject::connect(
		blurType,
		qOverload<int>(&QComboBox::currentIndexChanged),
		this,
		[this, slider, value](int index)
		{
			m_blurType = static_cast<BlurType>(index);
			const QSignalBlocker sliderBlocker(slider);
			slider->setValue(0);
			m_blurKernelSize = 1;
			value->setText("Kernel: 1");
			m_processedImage.release();
			m_selectedKeyPointIndex = -1;
			m_hasBlurPreview = false;
			m_keyPoints.clear();
			m_keyPointsDetected = false;
			if (m_showKeyPoints)
			{
				showKeyPoints();
			}
			else
			{
				displayCurrentImage();
			}
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
				m_showKeyPoints = false;
				displayCurrentImage();
			}
			
		}
	);

}

void MainWindow::showThresholdTools()
{
	if (m_thresholdSlider)
	{
		return;
	}

	QLabel* thresholdTitle = new QLabel("Threshold", toolsPanel);
	toolsLayout->addWidget(thresholdTitle);

	QComboBox* thresholdType = new QComboBox(toolsPanel);
	thresholdType->addItem("Binary");
	thresholdType->addItem("Binary Inverse");
	thresholdType->addItem("Trunc");
	thresholdType->addItem("To Zero");
	thresholdType->addItem("To Zero Inverse");
	toolsLayout->addWidget(thresholdType);

	QSlider* slider = new QSlider(Qt::Horizontal, toolsPanel);
	slider->setMinimum(0);
	slider->setMaximum(255);
	slider->setValue(m_thresholdValue);
	toolsLayout->addWidget(slider);
	m_thresholdSlider = slider;

	QLabel* value = new QLabel("Threshold: " + QString::number(m_thresholdValue), toolsPanel);
	toolsLayout->addWidget(value);
	m_thresholdValueLabel = value;

	QPushButton* resetButton = new QPushButton("Reset Image", toolsPanel);
	toolsLayout->addWidget(resetButton);
	QObject::connect(resetButton, &QPushButton::clicked, this, &MainWindow::resetThresholdImage);

	QObject::connect(
		thresholdType,
		qOverload<int>(&QComboBox::currentIndexChanged),
		this,
		[this](int index)
		{
			switch (index)
			{
			case 0: m_thresholdType = ThresholdType::Binary; break;
			case 1: m_thresholdType = ThresholdType::BinaryInverse; break;
			case 2: m_thresholdType = ThresholdType::Trunc; break;
			case 3: m_thresholdType = ThresholdType::ToZero; break;
			case 4: m_thresholdType = ThresholdType::ToZeroInverse; break;
			default: return;
			}
			const QSignalBlocker sliderBlocker(m_thresholdSlider);
			m_thresholdSlider->setValue(0);
			m_thresholdValue = 0;
			m_thresholdValueLabel->setText("Threshold: 0");
			applyThreshold();
		}
	);
	QObject::connect(
		slider,
		&QSlider::valueChanged,
		this,
		[this, value](int thresholdValue)
		{
			m_thresholdValue = thresholdValue;
			value->setText("Threshold: " + QString::number(m_thresholdValue));
			applyThreshold();
		}
	);
}

void MainWindow::applyBlur()
{
	if (!m_image)
	{
		return;
	}

	if (m_selectedKeyPointIndex >= 0 &&
		m_selectedKeyPointIndex < static_cast<int>(m_keyPoints.size()))
	{
		const cv::Rect selectedRegion = m_image->keyPointRegion(
			m_keyPoints[static_cast<std::size_t>(m_selectedKeyPointIndex)]
		);
		m_processedImage = m_image->applyBlurToRegion(
			selectedRegion,
			m_blurType,
			m_blurKernelSize,
			m_processedImage
		);
	}
	else
	{
		m_processedImage = m_image->applyBlur(m_blurType, m_blurKernelSize);
	}

	displayCurrentImage();
}

void MainWindow::applyThreshold()
{
	if (!m_image)
	{
		return;
	}

	Image thresholdInput(m_image->getImage());
	m_processedImage = thresholdInput.applyThreshold(m_thresholdType, m_thresholdValue);
	displayCurrentImage();
}

void MainWindow::resetThresholdImage()
{
	m_processedImage.release();
	m_thresholdValue = 0;
	if (m_thresholdSlider)
	{
		const QSignalBlocker blocker(m_thresholdSlider);
		m_thresholdSlider->setValue(0);
	}
	if (m_thresholdValueLabel)
	{
		m_thresholdValueLabel->setText("Threshold: 0");
	}
	displayCurrentImage();
}

void MainWindow::resetImage()
{
	m_processedImage.release();
	m_keyPoints.clear();
	m_selectedKeyPointIndex = -1;
	m_keyPointsDetected = false;
	m_showKeyPoints = false;
	resetBlurSlider();

	if (m_keyPointCheckBox)
	{
		const QSignalBlocker blocker(m_keyPointCheckBox);
		m_keyPointCheckBox->setChecked(false);
	}

	displayCurrentImage();
}

void MainWindow::resetBlurSlider()
{
	m_blurKernelSize = 1;
	m_hasBlurPreview = false;
	if (m_blurSlider)
	{
		const QSignalBlocker blocker(m_blurSlider);
		m_blurSlider->setValue(0);
	}
	if (m_kernelValueLabel)
	{
		m_kernelValueLabel->setText("Kernel: 1");
	}
}
