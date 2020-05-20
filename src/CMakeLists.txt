cmake_minimum_required(VERSION 3.2)
project(pulseview)

add_subdirectory(pulseview-core)
set(SOURCE_FILES main.cpp)

add_executable(pulseview ${SOURCE_FILES})
target_link_libraries(pulseview pulseview-core)
target_link_libraries(pulseview sfml-graphics)
target_link_libraries(pulseview sfml-system)
target_link_libraries(pulseview sfml-window)
install(TARGETS pulseview DESTINATION ${PULSEVIEW_INSTALL_BIN_DIR})
