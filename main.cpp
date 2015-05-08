#include <iostream>
#include <iomanip>
#include "FrameProcessor.h"
#include "SearchParameters.h"
#include "MotionExtractor.h"
#include "MosaicConstructor.h"


int main(int argc, char **argv) {

    if (!(argc == 4 || argc == 5)){
        std::cout << "Usage:   " << argv[0]
            << "   input_file  frame_description_file  search_parameters_file  [ number_of_frames_to_skip ]"
            << std::endl;
    }

    int frame_skip =1;
    if( argc == 5 ){
        frame_skip = std::stoi(argv[4]);
    }


    //Try and open the video/sequence that was passed
    cv::VideoCapture source_video(argv[1]);
    if (!source_video.isOpened()) {
        std::cout << "failed to open source footage\n";
        return -1;
    }

    //Grab the frame descriptor and search parameters from file
    FrameDescriptor frameDescriptor(argv[2]);
    SearchParameters search_parameters(argv[3]);


    //use the frame descriptor to initialise a frame processor object
    FrameProcessor frameProcessor(frameDescriptor);


    cv::Mat frame, input_frame;

    //OpenCV doesn't always read the first frame properly... keep grabbing frames until it does
    do {
        source_video >> input_frame;
    } while (input_frame.empty());


    //Make sure that the frame is greyscale
    if (input_frame.type() != CV_8UC1) {
        cv::cvtColor(input_frame, frame, CV_BGR2GRAY);
    } else {
        frame = input_frame;
    }


    //Get an idea of how many frames the video/sequence contains.
    long n_frames = (long)source_video.get(CV_CAP_PROP_FRAME_COUNT);


    //Create a motion extractor and mosaic constructor for each channel listed in the frame descriptor
    std::vector<MotionExtractor *> motionExtractors;
    std::vector<MosaicConstructor *> mosaicConstructors;

    for (int channel = 0; channel < frameDescriptor.get_channel_count(); channel++) {
        cv::Rect region = frameDescriptor.get_channel_regions().at(channel);
        motionExtractors.push_back(new MotionExtractor(region.size(), search_parameters));
        mosaicConstructors.push_back(new MosaicConstructor());
    }


    //Main loop!
    for (int i = 1; i < n_frames;) {

        //We use this loop to skip frames if the footage is a bit dense
        for (int f = 0; f < frame_skip; f++, i++) {
            source_video >> input_frame;

            //we also jump out of the main loop if we reach the end of the footage
            //(OpenCV doesnt always report frame count correctly)
            if (input_frame.empty()) {
                std::cout << "Reached end of footage!";
                goto END_PROCESSING;
            }
        }

        //Convert frames to greyscale if needed
        if (input_frame.type() != CV_8UC1) {
            cv::cvtColor(input_frame, frame, CV_BGR2GRAY);
        } else {
            frame = input_frame;
        }

        //Feed the frame processor a frame so it can split it into regions
        frameProcessor.insert(frame);

        //Initialise a Point to use as an accumulator for calculating average motion across the whole frame
        cv::Point average_motion(0, 0);

        //For each channel...
        for (int channel = 0; channel < frameDescriptor.get_channel_count(); channel++) {

            //...grab the current frame...
            MotionExtractor *current = motionExtractors.at(channel);
            current->insert(frameProcessor.extract_channel(channel), average_motion);

            //...and add its estimated translation to the accumulator.
            average_motion += current->get_translation();
        }

        //find the average by multiplying the accumulator by 1/number of channels (Points dont do division
        average_motion *= 1.0f / frameDescriptor.get_channel_count();

        //Add each region to the mosaic constructors
        for (int channel = 0; channel < frameDescriptor.get_channel_count(); channel++) {
            mosaicConstructors.at(channel)->insert(frameProcessor.extract_channel(channel), average_motion);
        }

        //Let us know roughly how far through we are
        std::cout << std::setprecision(3) << "Processing approximately " << 100 * (float) i / n_frames <<
        "% complete." << std::endl;
    }

    //Point to jump to when processing is over
    END_PROCESSING:

    //Write out the mosaics to the appropriate files
    for (int channel = 0; channel < frameDescriptor.get_channel_count(); channel++) {

        std::ostringstream output_filename;
        output_filename << frameDescriptor.get_channel_names().at(channel) << ".tiff";

        //grab the mosaic
        cv::Mat raw_mosaic = mosaicConstructors.at(channel)->extract();

        //work out how much it needs to be offset by to align it with the other mosaics
        cv::Point offset = frameDescriptor.get_channel_regions().at(channel).tl();

        //put the mosaic in an image the right size to account for this offset
        cv::Mat final_mosaic(raw_mosaic.rows + offset.y, raw_mosaic.cols + offset.x, CV_8UC1, cv::Scalar::all(0));
        raw_mosaic.copyTo(final_mosaic(cv::Rect(offset, raw_mosaic.size())));

        //write it out
        cv::imwrite(output_filename.str(), final_mosaic);
    }

    return 0;
}