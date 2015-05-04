#include <iostream>
#include <iomanip>
#include "FrameProcessor.h"
#include "SearchParameters.h"
#include "MotionExtractor.h"
#include "MosaicConstructor.h"

using namespace std;

int main(int argc, char **argv) {

    srand(time(NULL));

    int search_patch_size = 85;
    int search_window_size = 50;
    int max_search_points = 10;

    cv::VideoCapture source_video(argv[1]);
    if (!source_video.isOpened()) {
        cout << "failed to open source footage\n";
        return -1;
    }

    FrameDescriptor frameDescriptor(argv[2]);
    SearchParameters search_parameters(argv[3]);

    FrameProcessor frameProcessor(frameDescriptor);





    cv::Mat frame, input_frame;


    do {
        source_video >> input_frame;
    } while (input_frame.empty());

    if (input_frame.type() != CV_8UC1) {
        cv::cvtColor(input_frame, frame, CV_BGR2GRAY);
    } else {
        frame = input_frame;
    }


    long n_frames = source_video.get(CV_CAP_PROP_FRAME_COUNT);






    std::vector<MotionExtractor *> motionExtractors;
    std::vector<MosaicConstructor *> mosaicConstructors;

    for (int channel = 0; channel < frameDescriptor.get_channel_count(); channel++) {
        cv::Rect region = frameDescriptor.get_channel_regions().at(channel);
        motionExtractors.push_back(new MotionExtractor(region.size(), search_parameters));
        mosaicConstructors.push_back(new MosaicConstructor());

    }


    for (int i = 1; i < n_frames;) {


        //cv::waitKey(1);
        for (int f = 0; f < 1; f++, i++) {
            source_video >> input_frame;

            if (input_frame.empty()) {
                std::cout << "Reached end of footage!";
                goto END_PROCESSING;
            }
        }


        if (input_frame.type() != CV_8UC1) {
            cv::cvtColor(input_frame, frame, CV_BGR2GRAY);
        } else {
            frame = input_frame;
        }

        frameProcessor.insert(frame);

        cv::Point average_motion(0, 0);

        for (int channel = 0; channel < frameDescriptor.get_channel_count(); channel++) {
            MotionExtractor *current = motionExtractors.at(channel);
            current->insert(frameProcessor.extract_channel(channel), average_motion);
            average_motion += current->get_translation();
        }

        average_motion *= 1.0f / frameDescriptor.get_channel_count();

        for (int channel = 0; channel < frameDescriptor.get_channel_count(); channel++) {
            mosaicConstructors.at(channel)->insert(frameProcessor.extract_channel(channel), average_motion);
        }


        std::cout << std::setprecision(3) << "Processing approximately " << 100 * (float) i / n_frames <<
        "% complete." << std::endl;
    }

    END_PROCESSING:

    for (int channel = 0; channel < frameDescriptor.get_channel_count(); channel++) {
        std::ostringstream output_filename;

        output_filename << frameDescriptor.get_channel_names().at(channel) << ".tiff";

        cv::Mat raw_mosaic = mosaicConstructors.at(channel)->extract();

        cv::Point offset = frameDescriptor.get_channel_regions().at(channel).tl();

        cv::Mat final_mosaic(raw_mosaic.rows + offset.y, raw_mosaic.cols + offset.x, CV_8UC1, cv::Scalar::all(0));


        raw_mosaic.copyTo(final_mosaic(cv::Rect(offset, raw_mosaic.size())));

        cv::imwrite(output_filename.str(), final_mosaic);
    }


    return 0;
}