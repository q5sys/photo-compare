cmake_minimum_required(VERSION 3.16)
project(PhotoCompare)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt6_standard_project_setup()

set(SOURCES
    main.cpp
    mainwindow.cpp
    imagecomparewidget.cpp
)

set(HEADERS
    mainwindow.h
    imagecomparewidget.h
)

qt6_add_executable(PhotoCompare ${SOURCES} ${HEADERS})

target_link_libraries(PhotoCompare 
    PRIVATE 
    Qt6::Core 
    Qt6::Widgets
)
