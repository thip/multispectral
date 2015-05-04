//
// Created by David Capper on 03/05/2015.
//

#ifndef MAJOR_PROJECT_FINAL_SEARCHPARAMETERS_H
#define MAJOR_PROJECT_FINAL_SEARCHPARAMETERS_H


#include <opencv2/core/core.hpp>

class SearchParameters {

public:

    SearchParameters(std::string file_path);

    int max_points;
    cv::Size patch_size;
    cv::Size search_window_size;
    cv::Point expected_translation;
};


#endif //MAJOR_PROJECT_FINAL_SEARCHPARAMETERS_H
