cmake_minimum_required (VERSION 3.16)
include("LocalConfig.cmake")

project ("Task Manager")

find_package(Qt6 REQUIRED 
  COMPONENTS Widgets Gui Core
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON) 

add_executable(App main.cpp mainwindow.cpp mainwindow.h)

target_link_libraries(App PRIVATE Qt6::Core Qt6::Widgets)
