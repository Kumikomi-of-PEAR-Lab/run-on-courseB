cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 11)

# header
include_directories(include/)

# source file
FILE(GLOB SOURCE src/*)

# executable
add_executable(main ${SOURCE})

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

target_link_libraries(main ${OpenCV_LIBS})

