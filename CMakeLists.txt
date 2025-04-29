cmake_minimum_required (VERSION 3.16)
include("LocalConfig.cmake")

project ("Task Manager")

find_package(Qt6 REQUIRED 
  COMPONENTS Widgets Gui Core
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON) 

qt_add_executable(App
    mainwindow.cpp mainwindow.h mainwindow.ui
    main.cpp
    net.cpp
)

target_link_libraries(App PRIVATE Qt6::Core Qt6::Widgets)
