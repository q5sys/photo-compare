cmake_minimum_required(VERSION 3.16)
project(PhotoCompare)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt6_standard_project_setup()

set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/imagecomparewidget.cpp
)

set(HEADERS
    src/mainwindow.h
    src/imagecomparewidget.h
)

qt6_add_executable(PhotoCompare ${SOURCES} ${HEADERS})

target_link_libraries(PhotoCompare 
    PRIVATE 
    Qt6::Core 
    Qt6::Widgets
)

# Install executable
install(TARGETS PhotoCompare
    RUNTIME DESTINATION bin
)

# Install icon
install(FILES icons/PhotoCompare-256.png
    DESTINATION share/pixmaps
    RENAME PhotoCompare.png
)

# Install desktop file
install(FILES PhotoCompare.desktop
    DESTINATION share/applications
)
