//
// Created by David Capper on 10/04/2015.
//

#ifndef MULTISPECTRAL_MOSAICCONSTRUCTOR_H
#define MULTISPECTRAL_MOSAICCONSTRUCTOR_H


#include <opencv2/core/core.hpp>

//MosaicConstructor: turns regions from sequential frames into a mosaic
class MosaicConstructor {
public:
    MosaicConstructor();

    void insert( cv::Mat frame, cv::Point translation);
    cv::Mat extract();

private:
    cv::Point total_translation;
    cv::Point top_left, bottom_right;
    cv::Point background_position;
    cv::Mat mosaic;
};


#endif //MULTISPECTRAL_MOSAICCONSTRUCTOR_H
