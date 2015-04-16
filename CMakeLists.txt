cmake_minimum_required(VERSION 3.1)
project(multispectral)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)

add_executable(multispectral ${SOURCE_FILES}  MotionExtractor.cpp MotionExtractor.h FrameProcessor.cpp FrameProcessor.h MosaicConstructor.cpp MosaicConstructor.h FrameDescriptor.cpp FrameDescriptor.h)

target_link_libraries(
	multispectral
	opencv_core
	opencv_highgui
	opencv_stitching
	opencv_imgproc
)