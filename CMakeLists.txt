cmake_minimum_required(VERSION 3.16)

project(cinemareservation VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

# List all source files including headers and ui files
set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    login.cpp
    login.h
    login.ui
    signup.cpp
    signup.h
    signup.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(cinemareservation
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        cinema.h cinema.cpp cinema.ui
        booking.h booking.cpp booking.ui
    )
else()
    add_executable(cinemareservation ${PROJECT_SOURCES})
endif()

# Link Qt libraries (Widgets and Sql for database)
target_link_libraries(cinemareservation PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Sql)

# Set properties for macOS / Windows
set_target_properties(cinemareservation PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(cinemareservation)
endif()
