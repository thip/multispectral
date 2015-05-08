//
// Created by David Capper on 10/04/2015.
//

#include <iostream>

#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "MotionExtractor.h"

//Little helper function to calculate the sum of square differences between two patches
unsigned long sum_of_square_diffs(cv::Mat a, cv::Mat b) {

    unsigned long ssd = 0;

    for (int x = 0; x < a.cols; x++) {
        for (int y = 0; y < a.rows; y++) {
            ssd += pow(a.at<uchar>(x, y) - b.at<uchar>(x, y), 2);
        }
    }
    return ssd;
}


//estimates the motion between two frames, used as a helper but kept as a public member function in case it comes in handy
cv::Point MotionExtractor::get_motion_between(cv::Mat frameA, cv::Mat frameB) {

    //set up a vector to keep our feature points in
    std::vector<cv::Point> features;


    //calculate how big the borders need to be in order to stop us trying to look for features beyond the edges of frames
    int border_left, border_right, border_top, border_bottom;

    border_left = (search_parameters.search_window_size.width+1)/2 + (search_parameters.patch_size.width+1)/2 - search_parameters.expected_translation.x +10;
    border_right = (search_parameters.search_window_size.width+1)/2 + (search_parameters.patch_size.width+1)/2 + search_parameters.expected_translation.x +10;
    border_top = (search_parameters.search_window_size.height+1)/2 + (search_parameters.patch_size.height+1)/2 - search_parameters.expected_translation.y +10;
    border_bottom = (search_parameters.search_window_size.height+1)/2 + (search_parameters.patch_size.height+1)/2 + search_parameters.expected_translation.y +10;

    //if they've ended up being negative, just set them to 0
    border_left = border_left > 0 ? border_left : 0;
    border_right = border_right > 0 ? border_right : 0;
    border_top = border_top > 0 ? border_top : 0;
    border_bottom = border_bottom > 0 ? border_bottom : 0;

    //get a selection of featurepoints to track
    cv::goodFeaturesToTrack(frameA(cv::Rect(border_left, border_top, frameA.cols -  (border_left+border_right), frameA.rows - (border_top+border_bottom))),
                            features, search_parameters.max_points, 0.1, 10);

    //set up an accumulater for the average translation, and a vector to store individula translation estimates in
    cv::Point translation(0, 0);
    std::vector<cv::Point*> translations;

    //for each feature point...
    for (cv::Point point : features) {

        //offset it to account for the borders
        point += cv::Point(border_left, border_top);

        //set up something to keep track of the smallest result for sum of square difference...
        // set it initially to the biggest number possible
        unsigned long least(LONG_MAX);

        //Set up a pointer to refer to the best translation candidate
        cv::Point* best_translation;

        //set up a value to store the sum of squared differences calculation in
        unsigned long ssd;

        //Grab a patch centred around the feature point
        cv::Mat patch_a = frameA(
                cv::Range(point.y - search_parameters.patch_size.height / 2, point.y + search_parameters.patch_size.height / 2),
                cv::Range(point.x - search_parameters.patch_size.width / 2, point.x + search_parameters.patch_size.width / 2));

        //for every point in the search window
        for (int x = -search_parameters.search_window_size.width / 2; x < search_parameters.search_window_size.width / 2; x++)
            for (int y = -search_parameters.search_window_size.height / 2; y < search_parameters.search_window_size.height / 2; y++) {

                //grab a second patch around the point
                cv::Mat patch_b = frameB(
                        cv::Range(point.y + search_parameters.expected_translation.y - search_parameters.patch_size.width / 2 + y,
                                  point.y + search_parameters.expected_translation.y + search_parameters.patch_size.width / 2 + y),
                        cv::Range(point.x + search_parameters.expected_translation.x - search_parameters.patch_size.height / 2 + x,
                                  point.x + search_parameters.expected_translation.x + search_parameters.patch_size.height / 2 + x));

                //calculate the sum of squared differences
                ssd = sum_of_square_diffs(patch_b, patch_a);

                //if the sum of squared differences is less that the smallest one on record, update te record...
                if (ssd < least) {
                    least = ssd;

                    //and mark the current best estimate for translation as the difference between this point and the original one
                    best_translation = new cv::Point(search_parameters.expected_translation + cv::Point(x, y));
                }
            }

        //store the best estimate of translation for this feature and add it to the accumulator
        translations.push_back(best_translation);
        translation += *best_translation;
    }

    //make an initial calculation of the mean translation
    cv::Point mean_translation = translation * (1.0f / features.size());

    //sort all translation estimations based on their distance from the mean
    std::sort(translations.begin(), translations.end(), [mean_translation](cv::Point* a, cv::Point* b){
        auto distance_from_mean = [mean_translation](cv::Point* point){
            return (point->x - mean_translation.x)^2 * (point->y - mean_translation.y)^2;
        };

        return distance_from_mean(a) < distance_from_mean(b);
    });

    //reset the translation accumulator/mean
    translation = cv::Point(0, 0);

    //set up the cut off so that we only take the first 50% of translation estimates
    //(this is equivilent to the interquartile range seeing as we've basicly made all the differences in translation positive)
    unsigned long cutoff_index = translations.size()/2;

    //Take only the values that are in the IQR and clean up as we go
    for (int i = 0; i < cutoff_index; i++ ){
        translation += *translations.at(i);
        delete translations.at(i);
    }

    //Clean up the remainder
    for (int i = cutoff_index; i < translations.size(); i++)
    {
        delete translations.at(i);
    }

    //return the mean translation of all estimates that are in the IQR
    return translation * (1.0f/cutoff_index);
}

cv::Point const &MotionExtractor::get_translation() {
    return translation;
}

void MotionExtractor::insert(const cv::Mat &frame, cv::Point expected_translation) {
    search_parameters.expected_translation = expected_translation;
    translation = get_motion_between(frame, last_frame);
    frame.copyTo(last_frame);
}

MotionExtractor::MotionExtractor(cv::Size frame_size, SearchParameters search_parameters) : search_parameters(search_parameters) {
    last_frame = cv::Mat(frame_size, CV_8UC3);
}
