//
// Created by David Capper on 10/04/2015.
//

#ifndef MULTISPECTRAL_FRAMEDESCRIPTOR_H
#define MULTISPECTRAL_FRAMEDESCRIPTOR_H


#include <opencv2/core/core.hpp>

//FrameDescriptor: describes the regions that the filters create in frames
class FrameDescriptor {
public:
    FrameDescriptor(std::string file_path);
    FrameDescriptor(std::vector<cv::Rect> regions);

    std::vector<cv::Rect> const &get_channel_regions() const;
    std::vector<std::string> const &get_channel_names() const;
    unsigned long get_channel_count();

private:
    std::vector<std::string> channel_names;
    std::vector<cv::Rect> channel_regions;
};


#endif //MULTISPECTRAL_FRAMEDESCRIPTOR_H
