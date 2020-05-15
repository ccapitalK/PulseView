cmake_minimum_required(VERSION 3.2)
project(pulseview-tests)

add_subdirectory(lib/googletest)

include_directories(${PULSEVIEW_HEADERS_DIR})
include_directories(lib/googletest/googletest/include)

set(SOURCE_FILES main.cpp src/pulseview_tests.cpp)

add_executable(pulseview-tests ${SOURCE_FILES})
target_link_libraries(pulseview-tests pulseview-core gtest)
install(TARGETS pulseview-tests DESTINATION bin)

