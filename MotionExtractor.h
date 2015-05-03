//
// Created by David Capper on 10/04/2015.
//

#ifndef MULTISPECTRAL_MOTIONEXTRACTOR_H
#define MULTISPECTRAL_MOTIONEXTRACTOR_H


#include <opencv2/core/core.hpp>

struct Search_Parameters {
    int max_points;
    cv::Size patch_size;
    cv::Size search_window_size;
    cv::Point expected_translation;
};

class MotionExtractor {
public:
    MotionExtractor(cv::Size frame_size, int max_points, cv::Size search_patch_size, cv::Size search_window_size);

    void insert(const cv::Mat &frame, cv::Point expected_translation);
    cv::Point get_motion_between(cv::Mat frameA, cv::Mat frameB);

    cv::Point const & get_translation();

private:
    cv::Mat last_frame;
    cv::Point translation;
    Search_Parameters search_parameters;

};


#endif //MULTISPECTRAL_MOTIONEXTRACTOR_H
