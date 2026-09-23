#include "CameraInput.hpp"
#include <iostream>

cv::VideoCapture CameraInput::video;

bool CameraInput::init(int index, int width, int height) {
    video.open(index);
    if (!video.isOpened()) {
        std::cerr << "Error: Could not open the video camera stream.\n";
        return false;
    }

    video.set(cv::CAP_PROP_FRAME_WIDTH, width);
    video.set(cv::CAP_PROP_FRAME_HEIGHT, height);
    
    return true;
}


bool CameraInput::getFrame(cv::Mat& frame) {
    
    return video.read(frame) && !frame.empty();
}

void CameraInput::release() {
    if (video.isOpened()) {
        video.release();
    }
}
