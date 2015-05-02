#include <iostream>
#include "FrameProcessor.h"
#include "MotionExtractor.h"
#include "MosaicConstructor.h"

using namespace std;

int main(int argc, char** argv) {

    srand(time(NULL));


    cv::VideoCapture source_video(argv[1]);
    if ( !source_video.isOpened() ){
        cout<< "failed to open source footage\n";
        return -1;
    }

    cv::Mat frame, input_frame;


    do {
        source_video >> input_frame;
    }  while ( input_frame.empty() );

    cv::cvtColor(input_frame, frame, CV_BGR2GRAY);

    long n_frames = source_video.get(CV_CAP_PROP_FRAME_COUNT);

    std::vector<cv::Rect> regions;

    regions.push_back( cv::Rect(0,0,frame.cols, frame.rows/3 ) );
    regions.push_back( cv::Rect(0,frame.rows/3,frame.cols,frame.rows/3) );
    regions.push_back( cv::Rect(0,2*frame.rows/3,frame.cols, frame.rows/3) );

    //regions.push_back( cv::Rect(10,10,325, 470 ) );
    //regions.push_back( cv::Rect(370,10,630, 470 ) );

    cv::Size size = frame.size();

    FrameDescriptor frameDescriptor(regions, frame.size() );

    FrameProcessor frameProcessor( frameDescriptor );

    std::vector<MotionExtractor*> motionExtractors;
    std::vector<MosaicConstructor*> mosaicConstructors;

    for (int channel = 0; channel < frameDescriptor.getChannels(); channel++){
        motionExtractors.push_back(new MotionExtractor(frameDescriptor.get_regions().at(channel).size()));
        mosaicConstructors.push_back(new MosaicConstructor());
    }


    for (int i = 1; i < n_frames; ) {


        cv::waitKey(1);
        for (int f = 0; f <5; f++,i++){
            source_video >> input_frame;

            if (input_frame.empty()) {
                std::cout << "Reached end of footage earlier than expected!";
                goto END_PROCESSING;
            }
        }



        cv::cvtColor(input_frame, frame, CV_BGR2GRAY);

        frameProcessor.insert(frame);

        cv::Point average_motion;

        for (int channel = 0; channel < frameDescriptor.getChannels(); channel++){
            MotionExtractor* current = motionExtractors.at(channel);
            current->insert(frameProcessor.extract_channel(channel));
            average_motion += current->get_translation();
        }

        average_motion *= 1.0f/frameDescriptor.getChannels();

        for (int channel = 0; channel < frameDescriptor.getChannels(); channel++){
            mosaicConstructors.at(channel)->insert(frameProcessor.extract_channel(channel), average_motion);
        }



        std::cout << i << "/" << n_frames << std::endl;
    }

END_PROCESSING:

    cv::imshow("sup", mosaicConstructors.at(0)->extract());
    cv::waitKey(0);


    return 0;
}