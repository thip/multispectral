//
// Created by David Capper on 10/04/2015.
//

#ifndef MULTISPECTRAL_MOTIONEXTRACTOR_H
#define MULTISPECTRAL_MOTIONEXTRACTOR_H


#include <opencv2/core/core.hpp>

class MotionExtractor {
public:
    MotionExtractor(cv::Size frame_size);

    void insert(const cv::Mat &frame);
    cv::Point get_motion_between(cv::Mat frameA, cv::Mat frameB);

    cv::Point const & get_translation();

private:
    cv::Mat last_frame;
    cv::Point translation;

};


#endif //MULTISPECTRAL_MOTIONEXTRACTOR_H
