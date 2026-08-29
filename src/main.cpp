#include <QApplication>
#include <QWidget>
#include <QPushButton>

#include <iostream>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("CV Workbench");
    window.resize(800, 600);

    QPushButton button("Hello Qt", &window);
    button.setGeometry(350, 275, 100, 50);

    window.show();

    return app.exec();
}
