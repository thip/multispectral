//
// Created by David Capper on 10/04/2015.
//

#include <iostream>

#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "MotionExtractor.h"


struct search_parameters {
    cv::Size patch_size;
    cv::Size search_window_size;
    cv::Point expected_translation;
};

unsigned long sum_of_square_diffs( cv::Mat a, cv::Mat b ) {

    unsigned long ssd = 0;

    for ( int x = 0; x < a.cols; x++ ){
        for ( int y = 0; y < a.rows; y++ ){
            ssd += pow( a.at<uchar>(x,y) - b.at<uchar>(x,y) , 2 );
        }
    }
    return ssd;
}


cv::Point const & MotionExtractor::get_translation() {
    return translation;
}

void MotionExtractor::insert(const cv::Mat &frame) {
    translation = get_motion_between(frame, last_frame);
    //cv::imshow("a", last_frame); cv::imshow("b", frame); cv::waitKey(0);
    frame.copyTo(last_frame);
}

cv::Point MotionExtractor::get_motion_between(cv::Mat frameA, cv::Mat frameB) {

    ////////////////
    //more horror://
    ////////////////
    int number(50);
    std::vector<cv::Point> features;


    int border = 50;


    cv::goodFeaturesToTrack(frameA(cv::Rect(border, border, frameA.cols - 2*border, frameA.rows - 2*border)), features, number, 0.25, 10);

   /*for ( int i = 0; i < number; i++ ){
        features.push_back( cv::Point( rand() % ( frameA.cols - 2*border ) + border,
                                            rand() % ( frameA.rows - 2*border ) + border ) );

    }*/


    cv::Point translation(0, 0);

    cv::Point min(0,0), max(0,0);

    for ( cv::Point point : features ) {

        point += cv::Point(border,border);

        unsigned long least(LONG_MAX);


        cv::Point best_translation;
        unsigned long ssd;
        search_parameters params = {cv::Size(10,10), cv::Size(10,10), cv::Point(0,0)};

        cv::Mat patch_a = frameA(
                cv::Range(point.y - params.patch_size.height / 2, point.y + params.patch_size.height / 2),
                cv::Range(point.x - params.patch_size.width / 2, point.x + params.patch_size.width / 2));

        for (int x = -params.search_window_size.width / 2; x < params.search_window_size.width / 2; x++)
            for (int y = -params.search_window_size.height / 2; y < params.search_window_size.height / 2; y++) {
                cv::Mat patch_b = frameB(
                        cv::Range(point.y + params.expected_translation.y - params.patch_size.width / 2 + y,
                                  point.y + params.expected_translation.y + params.patch_size.width / 2 + y),
                        cv::Range(point.x + params.expected_translation.x - params.patch_size.height / 2 + x,
                                  point.x + params.expected_translation.x + params.patch_size.height / 2 + x));

                ssd = sum_of_square_diffs(patch_b, patch_a);
                if (ssd < least) {
                    least = ssd;

                    //is this point done the right way around?
                    best_translation = cv::Point(params.expected_translation + cv::Point(x, y));
                }
                //std::cout<< "L:" << least << std::endl;

            }

            translation += best_translation;
        }



 //return cv::Point(10,0);
     return translation * (1.0f/features.size());

}

MotionExtractor::MotionExtractor(cv::Size frame_size) {
    last_frame = cv::Mat(frame_size, CV_8UC3);
}
