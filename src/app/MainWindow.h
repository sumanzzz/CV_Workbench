#include "config.h"

#include <QMainWindow>

class QLabel;

class MainWindow :public QMainWindow
{
public:
	explicit MainWindow(QWidget* parent = nullptr);

	void openFile();

	QLabel* imageDisplay;
};
