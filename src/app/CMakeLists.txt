add_executable(CV_Workbench
    main.cpp
    Application.h
    Application.cpp
)

target_link_libraries(CV_Workbench
    PRIVATE
    Qt6::Widgets
    CV_Core
)