cmake_minimum_required (VERSION 3.16)
set(CURL_DIR "C:/vcpkg/installed/x64-windows/share/curl")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project ("Task Manager")

include_directories(curl)
add_executable(App main.cpp)

find_package(CURL REQUIRED)
target_link_libraries(App PRIVATE CURL::libcurl)