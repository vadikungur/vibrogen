cmake_minimum_required(VERSION 3.21)
project(vibrogen VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Widgets Multimedia)

qt_standard_project_setup()

add_executable(vibrogen
    src/main.cpp
    src/mainwindow.h
    src/mainwindow.cpp
    src/models.h
    src/signalengine.h
    src/signalengine.cpp
    src/projectserializer.h
    src/projectserializer.cpp
    src/signaldialog.h
    src/signaldialog.cpp
    src/scenariodialog.h
    src/scenariodialog.cpp
    src/trainconfigdialog.h
    src/trainconfigdialog.cpp
)

target_link_libraries(vibrogen PRIVATE Qt6::Widgets Qt6::Multimedia)

install(TARGETS vibrogen
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
