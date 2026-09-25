#include "config.h"

#include <QMainWindow>
#include <ImageOperations.h>

class QLabel;
class QVBoxLayout;

class MainWindow :public QMainWindow
{
public:
	explicit MainWindow(QWidget* parent = nullptr);

	void openFile();
	void displayImage(const cv::Mat& image);
	void showBlurTools();
	void applyBlur();

	QLabel* imageDisplay;
	QWidget* toolsPanel;
	QVBoxLayout* toolsLayout;

private:
	Image* m_image;
};
