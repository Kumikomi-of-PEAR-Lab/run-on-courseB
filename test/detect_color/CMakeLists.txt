cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 11)

# header
include_directories(../../include/)

# executable
add_executable(main test_detect_color.cpp ../../src/detect_color.cpp)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

target_link_libraries(main ${OpenCV_LIBS})

