#include "config.h"

#include <QMainWindow>
#include <QRect>
#include <ImageOperations.h>

#include <vector>

class QLabel;
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
	QRect m_displayRect;
	std::vector<cv::KeyPoint> m_keyPoints;
	int m_selectedKeyPointIndex = -1;

	void displayKeyPoints();
};
