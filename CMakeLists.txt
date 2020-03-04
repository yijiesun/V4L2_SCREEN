
cmake_minimum_required (VERSION 2.8)
project(SCREEN_SHOW)
add_definitions(-std=c++11)

set(CMAKE_BUILD_TYPE "Release")

set( SCREEN_SHOW_SRCS src/screen_show.cpp src/v4l2/v4l2.cpp src/screen/screen.cpp )
#opencv
find_package(OpenCV REQUIRED)

#exe
add_executable(SCREEN_SHOW ${SCREEN_SHOW_SRCS})
target_link_libraries(SCREEN_SHOW ${OpenCV_LIBS}  pthread)



