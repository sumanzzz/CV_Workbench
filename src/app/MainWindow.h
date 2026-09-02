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
	void showBlurTools();

	QLabel* imageDisplay;
	QWidget* toolsPanel;
	QVBoxLayout* toolsLayout;

private:
	Image* image;
};
