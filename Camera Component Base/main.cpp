#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "CameraInput.hpp"

int main()
{
    int width = 320;
    int height = 240;

    cv::Mat frame;
    cv::Mat gray;
    cv::Mat thresholded;

    cv::Mat labels;
    cv::Mat stats;
    cv::Mat centroids;

    int minArea = 200;
    int maxArea = 100000;


    if (!CameraInput::init(0, width, height)) {
        return 1;
    }

    while (CameraInput::getFrame(frame)) {

        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        cv::adaptiveThreshold(gray, thresholded, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 9, 2);

        int numComponents = cv::connectedComponentsWithStats(thresholded, labels, stats, centroids, 8, CV_32S);

        cv::Mat filteredBinary = cv::Mat::zeros(thresholded.size(), CV_8UC1);

        for (int i = 0; i < labels.rows; i++) {
            for (int c = 0; c < labels.cols; c++) {
                int label = labels.at<int>(i, c);
                if (label == 0) continue; 

                int area = stats.at<int>(label, cv::CC_STAT_AREA);

                if (area >= minArea && area <= maxArea) {
                    filteredBinary.at<uchar>(i, c) = 255;
                }
            }
        }

        cv::imshow("Camera Feed", filteredBinary);

        if (cv::waitKey(20) == 27) {
            break;
        }
    }

    CameraInput::release();
    cv::destroyAllWindows();
    return 0;
}
