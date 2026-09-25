#include "config.h"

#include <QMainWindow>
#include <QRect>
#include <ImageOperations.h>

#include <vector>

class QLabel;
class QCheckBox;
class QSlider;
class QVBoxLayout;

class MainWindow :public QMainWindow
{
public:
	explicit MainWindow(QWidget* parent = nullptr);

	void openFile();
	void displayImage(const cv::Mat& image);
	void showKeyPoints();
	void showBlurTools();
	void applyBlur();

	bool eventFilter(QObject* watched, QEvent* event) override;

	QLabel* imageDisplay;
	QWidget* toolsPanel;
	QVBoxLayout* toolsLayout;

private:
	Image* m_image = nullptr;
	cv::Mat m_processedImage;
	QRect m_displayRect;
	std::vector<cv::KeyPoint> m_keyPoints;
	int m_selectedKeyPointIndex = -1;
	BlurType m_blurType = BlurType::Gaussian;
	int m_blurKernelSize = 1;
	bool m_hasBlurPreview = false;
	bool m_keyPointsDetected = false;
	bool m_showKeyPoints = false;
	QCheckBox* m_keyPointCheckBox = nullptr;
	QSlider* m_blurSlider = nullptr;
	QLabel* m_kernelValueLabel = nullptr;

	void displayCurrentImage();
	void displayKeyPoints(const cv::Mat& baseImage);
	void resetBlurSlider();
	void resetImage();
	QRect mapSourceRectToDisplay(const cv::Rect& sourceRect) const;
};
