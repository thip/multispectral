//
// Created by David Capper on 03/05/2015.
//

#include <fstream>
#include "SearchParameters.h"

SearchParameters::SearchParameters(std::string file_path) {

    int patch_side_length, search_window_side_length;

    std::ifstream file(file_path);

    file >> max_points >> patch_side_length >> search_window_side_length;

    patch_size = cv::Size(patch_side_length, patch_side_length);
    search_window_size = cv::Size(search_window_side_length, search_window_side_length);
    expected_translation = cv::Point(0,0);
}