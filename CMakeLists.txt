# Set Name of project and language
project(PlanetStacker LANGUAGES CXX)

# Set cmake version
cmake_minimum_required(VERSION 3.0)


# Declare dependencies
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets Test Concurrent OpenGL)
find_package(OpenCV REQUIRED)
find_package(Threads)

set(OpenCV_DIR "C:/Users/basil/lib/opencv-build")
find_package(OpenCV REQUIRED COMPONENTS core imgproc highgui)
include_directories(${OpenCV_INCLUDE_DIRS})

# Qt Flags
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)
# Instruct CMake to get the resources
set(CMAKE_AUTORCC ON)


add_executable(${PROJECT_NAME}
    src/main.cpp
    src/app/planetprocessing.cpp
    src/app/planetprocessing.h
    src/app/iprocessing.h
    src/app/dataqueue.h
    src/ui/widget.cpp
    src/ui/widget.h
    src/ui/widget.ui
)

# Link used libraries
target_link_libraries(${PROJECT_NAME} PRIVATE Qt5::Widgets
                      ${OpenCV_LIBS}
                      ${CMAKE_THREAD_LIBS_INIT})

# set build type to Debug/Release
set(CMAKE_BUILD_TYPE "Debug")

#Set target properties (includes without specific path)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
    ${CMAKE_CURRENT_SOURCE_DIR}/src/app
    )


# Define required c++ standard to C++17
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_17)

# Set compile options, enable warnings
target_compile_options(${PROJECT_NAME} PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>: -Wall>
    $<$<CXX_COMPILER_ID:MSVC>: /W3>
)
