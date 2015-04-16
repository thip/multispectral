//
// Created by David Capper on 10/04/2015.
//

#include <iostream>
#include "FrameDescriptor.h"

std::vector<cv::Rect> const &FrameDescriptor::get_regions() const {
    return regions;
}

const cv::Size &FrameDescriptor::get_frame_size() const {
    return frame_size;
}

FrameDescriptor::FrameDescriptor(std::vector<cv::Rect> const &regions, cv::Size &frame_size) : regions(regions),
                                                                                               frame_size(frame_size) { }

unsigned long FrameDescriptor::getChannels() {
    return regions.size();
}

FrameDescriptor::FrameDescriptor(std::string string) {

    std::cout << "Not yet implemented!"<< std::endl;
}
