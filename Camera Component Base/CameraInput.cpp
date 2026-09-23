#include "CameraInput.hpp"
#include <iostream>

cv::VideoCapture CameraInput::video;
cv::Size frameSize;
cv::Mat videofeed;

bool CameraInput::init(int index, int width, int height) {

    frameSize = cv::Size(width, height);
    
    video.open(index);
    if (!video.isOpened()) {
        std::cerr << "Error: Could not open the video camera stream.\n";
        return false;
    }

    cv::namedWindow("Camera Feed", cv::WINDOW_NORMAL);
    cv::resizeWindow("Camera Feed", 640, 480); 

    return true;


}


bool CameraInput::getFrame(cv::Mat& frame) {
    

    if (!video.read(videofeed) || videofeed.empty()) {
        return false;
    }
    cv::resize(videofeed, frame, frameSize);
    return true;

}

void CameraInput::release() {
    if (video.isOpened()) {
        video.release();
    }
}
