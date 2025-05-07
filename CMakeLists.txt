cmake_minimum_required (VERSION 3.16)
include("LocalConfig.cmake")

project ("Task Manager")

find_package(Qt6 REQUIRED 
  COMPONENTS Widgets Gui Core
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON) 


qt_add_executable(App
    NetworkPerformanceItem.h
    NetworkPerformanceScanner.h
    mainwindow.cpp mainwindow.h mainwindow.ui
    main.cpp
    net.cpp
    process_info.cpp
    ProcessInfo.h

)
target_include_directories(App PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(App PRIVATE Qt6::Core Qt6::Widgets)

