//
// Created by David Capper on 10/04/2015.
//

#ifndef MULTISPECTRAL_FRAMEDESCRIPTOR_H
#define MULTISPECTRAL_FRAMEDESCRIPTOR_H


#include <opencv2/core/core.hpp>

class FrameDescriptor {
public:
    FrameDescriptor(std::string file_path);

    std::vector<cv::Rect> const &get_channel_regions() const;
    unsigned long get_channel_count();
    std::vector<std::string> const &getChannel_names() const;

    FrameDescriptor(std::vector<cv::Rect> regions);

private:
    std::vector<std::string> channel_names;
    std::vector<cv::Rect> channel_regions;

};


#endif //MULTISPECTRAL_FRAMEDESCRIPTOR_H
