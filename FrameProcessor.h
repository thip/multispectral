//
// Created by David Capper on 10/04/2015.
//

#ifndef MULTISPECTRAL_FRAMEPROCESSOR_H
#define MULTISPECTRAL_FRAMEPROCESSOR_H

#include "opencv2/opencv.hpp"
#include "FrameDescriptor.h"


class FrameProcessor {

public:

    FrameProcessor(FrameDescriptor frameDescriptor);

    void insert(cv::Mat frame);
    cv::Mat extract_channel(int channel);

private:
    std::vector<cv::Mat> channels;
    FrameDescriptor frameDescriptor;

    void remove_distortion(cv::Mat mat);
};


#endif //MULTISPECTRAL_FRAMEPROCESSOR_H
