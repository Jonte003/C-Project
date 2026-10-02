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

    int minArea = 500;
    int maxArea = 100000;

    int minQuadArea = 1000;

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

    std::vector<std::vector<cv::Point>> quadrilaterals;


    if (!CameraInput::init(0, width, height)) 
    {
        return 1;
    }

    while (CameraInput::getFrame(frame)) 
    {
        quadrilaterals.clear();

        cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

        cv::adaptiveThreshold(gray, thresholded, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY_INV, 9, 2);

        int numComponents = cv::connectedComponentsWithStats(thresholded, labels, stats, centroids, 8, CV_32S);

        cv::Mat filteredBinary = cv::Mat::zeros(thresholded.size(), CV_8UC1);

        for (int i = 1; i < numComponents; i++) 
        {
            int area = stats.at<int>(i, cv::CC_STAT_AREA);
            if (area >= minArea && area <= maxArea) 
            {
                filteredBinary.setTo(255, labels == i);
            }
        }        
        cv::findContours(filteredBinary, contours, hierarchy, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

        //cv::Mat CountorsFrame = frame.clone();
        //cv::drawContours(CountorsFrame, contours, -1, cv::Scalar(0, 255, 0), 2);

        for (auto& contour : contours) 
        {
        // Save all shapes that are convex, have 4 corners (quadrilaterals), and meet the minimum area size
            std::vector<cv::Point> approx;

            double perimeter = cv::arcLength(contour, true);

            cv::approxPolyDP(contour, approx, 0.02 * perimeter, true);
            double area = cv::contourArea(approx);

            if (approx.size() == 4 && cv::isContourConvex(approx) && area >= minQuadArea) 
            {
                quadrilaterals.push_back(approx);
            }
        }

        

        cv::Mat EdgesAndCorners = frame.clone();
        for (int i = 0; i < quadrilaterals.size(); ++i) 
        {
            cv::drawContours(EdgesAndCorners, quadrilaterals, static_cast<int>(i), cv::Scalar(255, 0, 0), 2);
            for (auto& corner : quadrilaterals[i]) 
            {
                cv::circle(EdgesAndCorners, corner, 4, cv::Scalar(0, 255, 255), -1);
            }
        }

        cv::imshow("Camera Feed", EdgesAndCorners);

        if (cv::waitKey(20) == 27) 
        {
            break;
        }
    }

    CameraInput::release();
    cv::destroyAllWindows();
    return 0;
}