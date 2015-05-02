//
// Created by David Capper on 10/04/2015.
//


#include "FrameProcessor.h"


void FrameProcessor::insert(cv::Mat frame) {

    remove_distortion(frame);

    for ( cv::Rect region : frameDescriptor.get_regions() ){
        channels.push_back(frame(region));
    }
}

cv::Mat FrameProcessor::extract_channel(int channel) {
    return channels.at(channel);
}

int FrameProcessor::channel_count() {
    return channels.size();
}

FrameProcessor::FrameProcessor(FrameDescriptor frameDescriptor) : frameDescriptor(frameDescriptor) {
}

void FrameProcessor::remove_distortion(cv::Mat mat) {
    std::cout << "Distortion removal not implemented yet!" << std::endl;
    return;
}
