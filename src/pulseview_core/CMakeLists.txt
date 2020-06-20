cmake_minimum_required(VERSION 3.2)
project(pulseview-core C CXX)

set(SOURCE_FILES
    application.cpp
    fftw_helper.cpp
    pcm_process_source.cpp
    pulseaudio_source.cpp
    pulseview.cpp
    render_model.cpp
)

add_library(pulseview-core SHARED STATIC ${SOURCE_FILES})

install(TARGETS pulseview-core DESTINATION ${PULSEVIEW_INSTALL_LIB_DIR})
install(FILES pulseview.h DESTINATION ${PULSEVIEW_INSTALL_INCLUDE_DIR})
