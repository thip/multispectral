//
// Created by David Capper on 10/04/2015.
//

#include <iostream>
#include <fstream>
#include "FrameDescriptor.h"

std::vector<cv::Rect> const &FrameDescriptor::get_channel_regions() const {
    return channel_regions;
}

FrameDescriptor::FrameDescriptor(std::vector<cv::Rect> regions) : channel_regions(regions) { }

unsigned long FrameDescriptor::get_channel_count() {
    return channel_regions.size();
}

std::vector<std::string> const &FrameDescriptor::getChannel_names() const {
    return channel_names;
}

FrameDescriptor::FrameDescriptor(std::string file_path) {

    std::string channel_name;
    int x1, y1, x2, y2;
    int channel_count;

    std::ifstream file(file_path);

    file >> channel_count;

    std::string line;
    for( int i = 0; i < channel_count; i++){

        std::getline(file, line, ';');

        std::stringstream input_string(line);

        input_string >> channel_name >> x1 >> y1 >> x2 >> y2;

        std::cout<<"Loaded region: " << channel_name << std::endl;

        channel_names.push_back(channel_name);
        channel_regions.push_back(cv::Rect(cv::Point(x1,y1), cv::Point(x2,y2)));
    }
}
