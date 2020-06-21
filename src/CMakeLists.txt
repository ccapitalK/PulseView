cmake_minimum_required(VERSION 3.2)
project(pulseview)

add_subdirectory(pulseview_core)
add_subdirectory(cxxopts)

include_directories(cxxopts/include)

set(SOURCE_FILES main.cpp)

add_executable(pulseview ${SOURCE_FILES})
target_link_libraries(pulseview dl)
target_link_libraries(pulseview fftw3)
target_link_libraries(pulseview pthread)
target_link_libraries(pulseview pulse)
target_link_libraries(pulseview pulse-simple)
target_link_libraries(pulseview pulseview-core)
target_link_libraries(pulseview sfml-graphics)
target_link_libraries(pulseview sfml-system)
target_link_libraries(pulseview sfml-window)
install(TARGETS pulseview DESTINATION ${PULSEVIEW_INSTALL_BIN_DIR})
