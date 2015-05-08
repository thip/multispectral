//
// Created by David Capper on 10/04/2015.
//


#include "FrameProcessor.h"


void FrameProcessor::insert(cv::Mat frame) {
    for ( cv::Rect region : frameDescriptor.get_channel_regions() ){
        channels.push_back(frame(region));
    }
}

cv::Mat FrameProcessor::extract_channel(unsigned long channel) {
    return channels.at(channel);
}


FrameProcessor::FrameProcessor(FrameDescriptor frameDescriptor) : frameDescriptor(frameDescriptor) {
}


