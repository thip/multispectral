//
// Created by David Capper on 10/04/2015.
//

#ifndef MULTISPECTRAL_FRAMEPROCESSOR_H
#define MULTISPECTRAL_FRAMEPROCESSOR_H

#include "opencv2/opencv.hpp"
#include "FrameDescriptor.h"


//FrameProcessor: Takes frames and splits them into the regions made by the filters
class FrameProcessor {

public:

    FrameProcessor(FrameDescriptor frameDescriptor);

    void insert(cv::Mat frame);
    cv::Mat extract_channel(unsigned long channel);

private:
    std::vector<cv::Mat> channels;
    FrameDescriptor frameDescriptor;
};


#endif //MULTISPECTRAL_FRAMEPROCESSOR_H
