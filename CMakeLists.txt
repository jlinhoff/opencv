cmake_minimum_required(VERSION 2.8)
project( simplecv )
find_package( OpenCV REQUIRED )

add_executable( simplecv src/main.cpp src/iorect.cpp src/iofill.cpp )
target_link_libraries( simplecv ${OpenCV_LIBS} )

SET(GCC_COVERAGE_COMPILE_FLAGS "-fpermissive -std=c++11")
SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
