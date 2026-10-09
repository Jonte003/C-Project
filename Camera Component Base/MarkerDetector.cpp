#include "MarkerDetector.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>

static void SortCorners(std::vector<std::vector<cv::Point>>& quadrilaterals) 
{
    for (auto& quad : quadrilaterals) 
    {
        // seperate top and bottom points by y-coordinate
        std::sort(quad.begin(), quad.end(), [](const cv::Point& a, const cv::Point& b) {
            return a.y < b.y;
        });

        cv::Point topLeft, topRight, bottomRight, bottomLeft;

        // Top row
        if (quad[0].x < quad[1].x) {
            topLeft  = quad[0];
            topRight = quad[1];
        } else {
            topLeft  = quad[1];
            topRight = quad[0];
        }

        // Bottom row
        if (quad[2].x < quad[3].x) {
            bottomRight = quad[3];
            bottomLeft  = quad[2];
        } else {
            bottomRight = quad[2];
            bottomLeft  = quad[3];
        }

        quad[0] = topLeft;
        quad[1] = topRight;
        quad[2] = bottomRight;
        quad[3] = bottomLeft;
    }
}

void MarkerDetector::detectMarkers(cv::Mat& frame, int minArea, int maxArea, int minQuadArea, std::vector<std::vector<cv::Point>>& quadrilaterals)
{
    cv::Mat gray, thresholded, labels, stats, centroids;
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

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

        SortCorners(quadrilaterals);

        //Print corners of found quadrilaterals
        for (int i = 0; i < quadrilaterals.size(); ++i) 
        {
            std::cout << "Quadrilateral " << i << std::endl;
            for (int j = 0; j < quadrilaterals[i].size(); ++j) 
            {
                std::cout << quadrilaterals[i][j].x << ", " << quadrilaterals[i][j].y << std::endl;
            }
        }
}


