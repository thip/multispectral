//
// Created by David Capper on 10/04/2015.
//

#include <iostream>
#include <opencv2/highgui/highgui.hpp>
#include "MosaicConstructor.h"


void MosaicConstructor::insert(cv::Mat frame, cv::Point translation) {

    //update the cumulative translation with the of the incoming frame
    total_translation += translation;

    //reset the background position
    background_position = cv::Point(0,0);

    //Find out whether translation has taken the frame beyond the top or left edges of the mosaic
    //if it has, change the background position so that the frame can be drawn at 0,0 and the background
    //moved as appropriate. Also set the coordinates of the top left and bottom right corners of the mosaic
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


    //create a new material that is large enough to contain both the frame and the background,
    cv::Mat new_mosaic( cv::Size( bottom_right.x - top_left.x , bottom_right.y - top_left.y) + frame.size(), CV_8UC1, cv::Scalar::all(0));

    //set up Rects describing the bounds of the various components that will be assembled into the mosaic
    cv::Rect mosaic_bounds ( cv::Point(0,0) , new_mosaic.size() );
    cv::Rect frame_bounds (  total_translation - top_left, frame.size());
    cv::Rect background_bounds ( background_position, mosaic.size() );

    //Trim the bounds so we dont get any nasty surprises is something has ended up out of bounds somehow
    cv::Rect trimmed_bounds( background_bounds & mosaic_bounds );

    //trim the current mosaic to size....
    mosaic = mosaic(cv::Rect(cv::Point(0,0), trimmed_bounds.size())).clone();

    //and copy it to the new mosaic
    mosaic(cv::Rect(cv::Point(0,0), trimmed_bounds.size())).copyTo(new_mosaic( cv::Rect( background_position, trimmed_bounds.size()) )   );

    //copy the current frame over to it too, making sure it doesnt go out of bounds
    frame.copyTo( new_mosaic( frame_bounds & mosaic_bounds ) );

    //update the mosaic so that the new one is used.
    mosaic = new_mosaic.clone();

    //cv::imshow("Helpful debugging window", mosaic);
}

cv::Mat MosaicConstructor::extract() {
    return mosaic;
}

MosaicConstructor::MosaicConstructor() {
    mosaic = cv::Mat(1,1,CV_8UC1);
    total_translation = cv::Point(0,0);
    background_position = cv::Point(0,0);
    top_left = cv::Point(0,0);
    bottom_right = cv::Point(0,0);

}
