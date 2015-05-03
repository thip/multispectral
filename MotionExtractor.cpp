//
// Created by David Capper on 10/04/2015.
//

#include <iostream>

#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "MotionExtractor.h"




unsigned long sum_of_square_diffs(cv::Mat a, cv::Mat b) {

    unsigned long ssd = 0;

    for (int x = 0; x < a.cols; x++) {
        for (int y = 0; y < a.rows; y++) {
            ssd += pow(a.at<uchar>(x, y) - b.at<uchar>(x, y), 2);
        }
    }
    return ssd;
}


cv::Point const &MotionExtractor::get_translation() {
    return translation;
}

void MotionExtractor::insert(const cv::Mat &frame, cv::Point expected_translation) {
    search_parameters.expected_translation = expected_translation;
    translation = get_motion_between(frame, last_frame);
    frame.copyTo(last_frame);
}

cv::Point MotionExtractor::get_motion_between(cv::Mat frameA, cv::Mat frameB) {



    std::vector<cv::Point> features;


    int border_left, border_right, border_top, border_bottom;

    border_left = (search_parameters.search_window_size.width+1)/2 + (search_parameters.patch_size.width+1)/2 - search_parameters.expected_translation.x +10;
    border_right = (search_parameters.search_window_size.width+1)/2 + (search_parameters.patch_size.width+1)/2 + search_parameters.expected_translation.x +10;
    border_top = (search_parameters.search_window_size.height+1)/2 + (search_parameters.patch_size.height+1)/2 - search_parameters.expected_translation.y +10;
    border_bottom = (search_parameters.search_window_size.height+1)/2 + (search_parameters.patch_size.height+1)/2 + search_parameters.expected_translation.y +10;

    border_left = border_left > 0 ? border_left : 0;
    border_right = border_right > 0 ? border_right : 0;
    border_top = border_top > 0 ? border_top : 0;
    border_bottom = border_bottom > 0 ? border_bottom : 0;

    cv::goodFeaturesToTrack(frameA(cv::Rect(border_left, border_top, frameA.cols -  (border_left+border_right), frameA.rows - (border_top+border_bottom))),
                            features, search_parameters.max_points, 0.1, 10);



    cv::Point translation(0, 0);


    for (cv::Point point : features) {

        point += cv::Point(border_left, border_top);

        unsigned long least(LONG_MAX);


        cv::Point best_translation;
        unsigned long ssd;


        cv::Mat patch_a = frameA(
                cv::Range(point.y - search_parameters.patch_size.height / 2, point.y + search_parameters.patch_size.height / 2),
                cv::Range(point.x - search_parameters.patch_size.width / 2, point.x + search_parameters.patch_size.width / 2));

        for (int x = -search_parameters.search_window_size.width / 2; x < search_parameters.search_window_size.width / 2; x++)
            for (int y = -search_parameters.search_window_size.height / 2; y < search_parameters.search_window_size.height / 2; y++) {
                cv::Mat patch_b = frameB(
                        cv::Range(point.y + search_parameters.expected_translation.y - search_parameters.patch_size.width / 2 + y,
                                  point.y + search_parameters.expected_translation.y + search_parameters.patch_size.width / 2 + y),
                        cv::Range(point.x + search_parameters.expected_translation.x - search_parameters.patch_size.height / 2 + x,
                                  point.x + search_parameters.expected_translation.x + search_parameters.patch_size.height / 2 + x));

                ssd = sum_of_square_diffs(patch_b, patch_a);
                if (ssd < least) {
                    least = ssd;


                    best_translation = cv::Point(search_parameters.expected_translation + cv::Point(x, y));
                }


            }

        translation += best_translation;
    }




    return translation * (1.0f / features.size());

}

MotionExtractor::MotionExtractor(cv::Size frame_size, int max_points, cv::Size search_patch_size, cv::Size search_window_size) {
    last_frame = cv::Mat(frame_size, CV_8UC3);

    search_parameters = {max_points, search_patch_size, search_window_size, cv::Point(0, 0)};
}
