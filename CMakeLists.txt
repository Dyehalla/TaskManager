cmake_minimum_required (VERSION 3.16)
include("LocalConfig.cmake")

project ("Task Manager")

find_package(Qt6 REQUIRED 
  COMPONENTS Widgets Gui Core Network
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
    virustotal.cpp
    virustotal.h
    vt_dialog.cpp vt_dialog.h vt_dialog.ui
    timer_dialog.ui
    timer_dialog.cpp
    timer_dialog.h
)

target_include_directories(App PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(App PRIVATE Qt6::Core Qt6::Widgets Qt6::Network)
set_target_properties(App PROPERTIES
    WIN32_EXECUTABLE TRUE
)
