#include <iostream>
#include "FrameProcessor.h"
#include "MotionExtractor.h"
#include "MosaicConstructor.h"

using namespace std;

int main(int argc, char** argv) {

    srand(time(NULL));


    cv::VideoCapture source_video(argv[1]);

    cv::Mat frame;

    do {
        source_video >> frame;
    }  while ( frame.empty() );

   // cv::cvtColor(first_frame, frame, CV_BGR2GRAY);

    long n_frames = 3616;//(long) source_video.get(CV_CAP_PROP_FRAME_COUNT) -1;

    std::vector<cv::Rect> regions;

    regions.push_back( cv::Rect(0,0,frame.cols, frame.rows/3 ) );
    regions.push_back( cv::Rect(0,frame.rows/3,frame.cols,frame.rows/3) );
    regions.push_back( cv::Rect(0,2*frame.rows/3,frame.cols, frame.rows/3) );

    cv::Size size = frame.size();

    FrameDescriptor frameDescriptor(regions, size);

    FrameProcessor frameProcessor( frameDescriptor );

    MotionExtractor motion( regions.at(0).size() );
    MosaicConstructor mosaic;

    for (int i = 1; i < n_frames; i++) {

        //cv::cvtColor(source_video.grab(), frame, CV_BGR2GRAY);

        cv::waitKey(1);
        for (int f = 0; f <1; f++) source_video >> frame;

        frameProcessor.insert(frame);


        motion.insert(frameProcessor.extract_channel(0));

        std::cout << i << "/" << n_frames << std::endl;

        mosaic.insert(frameProcessor.extract_channel(0), motion.get_translation(), motion.get_transformation());






    }
    cv::imshow("sup", mosaic.extract());
    cv::waitKey(0);


    return 0;
}