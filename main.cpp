#include <iostream>
#include <opencv2/opencv.hpp>

int main()
{
    cv::VideoCapture video("resources/camera_test.mp4");
    if (!video.isOpened()) {
        std::cerr << "Could not open the video.\n";
        return 1;
    }

    cv::Mat frame;
    while (video.read(frame)) {
        cv::imshow("OpenCV video test", frame);
        if (cv::waitKey(20) == 27) {
            break;
        }
    }
    return 0;
}