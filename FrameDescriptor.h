//
// Created by David Capper on 10/04/2015.
//

#ifndef MULTISPECTRAL_FRAMEDESCRIPTOR_H
#define MULTISPECTRAL_FRAMEDESCRIPTOR_H


#include <opencv2/core/core.hpp>

class FrameDescriptor {
public:
    FrameDescriptor(std::string string);

    const cv::Size &get_frame_size() const;
    std::vector<cv::Rect> const & get_regions() const;
    unsigned long getChannels();

    FrameDescriptor(std::vector<cv::Rect> const &regions, cv::Size &frame_size);

private:
    const cv::Size frame_size;
    const std::vector<cv::Rect> regions;

};


#endif //MULTISPECTRAL_FRAMEDESCRIPTOR_H
