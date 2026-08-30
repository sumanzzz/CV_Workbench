#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QDebug>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>

#include <QFileDialog>

#include <opencv2/opencv.hpp>

#include <iostream>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle("CV Workbench");
    window.resize(800, 600);

    QMenu* fileMenu = window.menuBar()->addMenu("File");
    QAction* openAction = fileMenu->addAction("Open");

    window.statusBar()->showMessage("Ready");

    QWidget* centralWindow = new QWidget(&window);

    window.setCentralWidget(centralWindow);

    QGridLayout layout(centralWindow);

    QPushButton button("Click",centralWindow);
    button.setGeometry(350, 275, 100, 50);
    button.move(375, 200);

    QLabel label("Write Something", centralWindow);
    label.resize(500, 30);
    

    QLineEdit input(centralWindow);
    
    layout.addWidget(&label,0,0,1,2);
    
    layout.addWidget(&input,1,0);
    layout.addWidget(&button, 1,1);

    

    QObject::connect
    (
        &button, &QPushButton::clicked, []()
        {
            qDebug() << "Holy fuck this button works...Fuckin A ...label shud be changed fuckin";
        }
    );

    
    QObject::connect
    (
        &button, &QPushButton::clicked, [&label , &input]()
        {
            label.setText(input.text());
        }
    );
    QObject::connect
    (
        openAction, &QAction::triggered, [&window]()
        {
            QString filename = QFileDialog::getOpenFileName
            (
                &window,
                "Open File",
                "",
                "Images(*.png *.jpg *.jpeg *.bmp)"
            );
            if (filename.isEmpty())
            {
                return;
            }
            cv::Mat image = cv::imread(filename.toStdString());
            if (image.empty())
            {
                qDebug() << "Failed to load image";
                return;
            }
            
            qDebug() << "Image Loaded :"<<image.cols<<"x"<<image.rows;
        }
    );

    window.show();

    return app.exec();
}
