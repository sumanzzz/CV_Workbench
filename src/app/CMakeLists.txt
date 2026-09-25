add_executable(CV_Workbench
    main.cpp
    Application.h
    Application.cpp
 "config.h"  "MainWindow.h" "MainWindow.cpp"
    ../../resources/resources.qrc)

target_link_libraries(CV_Workbench
    PRIVATE
    Qt6::Widgets
    CV_Core
)
