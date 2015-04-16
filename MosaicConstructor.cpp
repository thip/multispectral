//
// Created by David Capper on 10/04/2015.
//

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include "MosaicConstructor.h"

using namespace cv;

void MosaicConstructor::insert(cv::Mat frame, cv::Point translation, cv::Mat transformation) {
    total_translation += translation;
    background_position = Point(0,0);
    
    if (top_left.x >  total_translation.x) {
        top_left.x = total_translation.x;
        background_position.x = -translation.x;
    }

    if (top_left.y >  total_translation.y) {
        top_left.y = total_translation.y;
        background_position.y = -translation.y;
    }


    bottom_right.x = bottom_right.x > total_translation.x ? bottom_right.x : total_translation.x;
    bottom_right.y = bottom_right.y > total_translation.y ? bottom_right.y : total_translation.y;

    //create a new material that can contain both the frame and the background, and give it a 10px border
    cv::Mat temp_mosaic ( cv::Size( bottom_right.x - top_left.x , bottom_right.y - top_left.y) + frame.size(), CV_8UC3, cv::Scalar::all(0));

    std::cout << background_position  << "\n";

    cv::Rect mosaic_bounds ( cv::Point(0,0) , temp_mosaic.size() );
    cv::Rect frame_bounds (  total_translation - top_left, frame.size());
    cv::Rect background_bounds ( background_position, mosaic.size() );

    cv::Rect a_rect( cv::operator&(background_bounds, mosaic_bounds) );

    mosaic = mosaic(Rect(Point(0,0), a_rect.size())).clone();

    mosaic(Rect(Point(0,0), a_rect.size())).copyTo( temp_mosaic( Rect( background_position, a_rect.size()) )   );
    frame.copyTo( temp_mosaic( frame_bounds & mosaic_bounds ) );


    mosaic = temp_mosaic.clone();
    cv::imshow(":(", mosaic);
}

cv::Mat MosaicConstructor::extract() {
    return mosaic;
}

MosaicConstructor::MosaicConstructor() {
    mosaic = cv::Mat(10,10,CV_8UC3);
    total_translation = Point(0,0);
    background_position = Point(0,0);
    top_left = Point(0,0);
    bottom_right = Point(0,0);

}
