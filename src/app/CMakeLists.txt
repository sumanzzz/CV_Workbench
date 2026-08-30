add_executable(CV_Workbench
    main.cpp   
)

target_include_directories(CV_Workbench
    PRIVATE
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(CV_Workbench
    PRIVATE
    ${OpenCV_LIBS}
    Qt6::Widgets
)