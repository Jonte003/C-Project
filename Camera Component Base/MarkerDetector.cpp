#include "MarkerDetector.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>

class QuadWithMat 
{
    public:
        std::vector<cv::Point> corners;
        cv::Mat matrix;
};

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

static std::vector<QuadWithMat> GetAllWarpedMarkers(cv::Mat& sourceFrame, std::vector<std::vector<cv::Point>>& allSortedQuads) 
{

    if (allSortedQuads.empty()) {
        return std::vector<QuadWithMat>();
    }


    std::vector<QuadWithMat> warpedMarkers;

    int markerSize = 500;
    float padding = 20.0f; //White padding around marker requiered for ArUco detection. 
    float totalSize = static_cast<float>(markerSize) + (2.0f * padding);

    std::vector<cv::Point2f> dstCorners = 
    {
        cv::Point2f(padding, padding),
        cv::Point2f(totalSize - padding, padding),
        cv::Point2f(totalSize - padding, totalSize - padding),
        cv::Point2f(padding, totalSize - padding) 
    };

    for (auto& quad : allSortedQuads) 
    {
        std::vector<cv::Point2f> cornersFloat(quad.begin(), quad.end());

        cv::Mat transformMatrix = cv::getPerspectiveTransform(cornersFloat, dstCorners);

        cv::Mat warpedOutput;
        cv::warpPerspective(sourceFrame, warpedOutput, transformMatrix, cv::Size(static_cast<int>(totalSize), static_cast<int>(totalSize)), cv::INTER_LINEAR, cv::BORDER_CONSTANT, cv::Scalar(255, 255, 255));
        
        warpedMarkers.push_back({quad, warpedOutput});
    }
    
    return warpedMarkers;
}

static std::vector<QuadWithID> PrintWarpedArucoCodes(std::vector<QuadWithMat>& warpedMarkers) 
{
    cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
    cv::aruco::DetectorParameters detectorParams = cv::aruco::DetectorParameters();
    cv::aruco::ArucoDetector detector(dictionary, detectorParams);

    std::vector<QuadWithID> detectedMarkers;


    for (size_t i = 0; i < warpedMarkers.size(); ++i) 
    {
        std::vector<std::vector<cv::Point2f>> markerCorners;
        std::vector<int> markerIds;
        std::vector<std::vector<cv::Point2f>> rejectedCandidates;

        cv::Mat gray;

        cv::cvtColor(warpedMarkers[i].matrix, gray, cv::COLOR_BGR2GRAY);

        detector.detectMarkers(gray, markerCorners, markerIds, rejectedCandidates);

        if (!markerIds.empty())
        {
            detectedMarkers.push_back({warpedMarkers[i].corners, markerIds[0]});
        }
  
    }
    
    return detectedMarkers;
}



std::vector<QuadWithID> MarkerDetector::detectMarkers(cv::Mat& frame, int minArea, int maxArea, int minQuadArea)
{
        std::vector<QuadWithID> detectedMarkers;
        std::vector<std::vector<cv::Point>> quadrilaterals;
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

        std::vector<QuadWithMat> warpedMarkers = GetAllWarpedMarkers(frame, quadrilaterals);
        detectedMarkers = PrintWarpedArucoCodes(warpedMarkers);

        return detectedMarkers;
}



