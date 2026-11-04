#ifndef CAMERA_INPUT_HPP
#define CAMERA_INPUT_HPP

#include <opencv2/opencv.hpp>

class CameraInput {
private:
    static cv::VideoCapture video;

public:
    static bool init(int index, int width, int height);

    // Static method to grab the next frame
    static bool getFrame(cv::Mat& frame);

    // Static method to clean up resources
    static void release();
};

#endif