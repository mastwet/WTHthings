cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PortMidi REQUIRED)
find_package(FLTK REQUIRED COMPONENTS all)

# 设置 PortAudio 的头文件路径和库路径
include_directories(/opt/homebrew/Cellar/portaudio/19.7.0/include)
link_directories(/opt/homebrew/Cellar/portaudio/19.7.0/lib)

include_directories(/opt/homebrew/Cellar/portmidi/2.0.4_1/include)
link_directories(/opt/homebrew/Cellar/portmidi/2.0.4_1/lib)

find_package(FLTK REQUIRED COMPONENTS all)
if(FLTK_FOUND)
    include_directories(${FLTK_INCLUDE_DIR})
    add_definitions(${FLTK_DEFINITIONS})
endif()

# 添加可执行文件
add_executable(MyApp main.cpp
        AudioEngine.cpp
        MidiHandler.cpp
        SynthGUI.cpp
        dsp.h
        Parameters.h)

# 正确链接 PortAudio 库和 macOS 框架
target_link_libraries(MyApp
        ${FLTK_LIBRARIES}
        PortMidi
        portaudio
        "-framework CoreAudio"
        "-framework AudioToolbox"
        "-framework AudioUnit"
        "-framework CoreFoundation"
        "-framework CoreServices")
