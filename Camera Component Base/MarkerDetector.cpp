#include "MarkerDetector.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>

/*
Component: MarkerDetector
Primary owner: Jonathan Hettinger
Responsibility: Find contours and return marker corner positions and ArUco id.
Main contributions: Countor detection, Corner flattening, ArUco code reading
*/


class QuadWithMat
{
public:
    std::vector<cv::Point> corners;
    cv::Mat matrix;
};

static void SortCorners(std::vector<std::vector<cv::Point>> &quadrilaterals)
{
    for (auto &quad : quadrilaterals)
    {
        // seperate top and bottom points by y-coordinate
        std::sort(quad.begin(), quad.end(), [](const cv::Point &a, const cv::Point &b)
                  { return a.y < b.y; });

        cv::Point topLeft, topRight, bottomRight, bottomLeft;

        // Top row
        if (quad[0].x < quad[1].x)
        {
            topLeft = quad[0];
            topRight = quad[1];
        }
        else
        {
            topLeft = quad[1];
            topRight = quad[0];
        }

        // Bottom row
        if (quad[2].x < quad[3].x)
        {
            bottomRight = quad[3];
            bottomLeft = quad[2];
        }
        else
        {
            bottomRight = quad[2];
            bottomLeft = quad[3];
        }

        quad[0] = topLeft;
        quad[1] = topRight;
        quad[2] = bottomRight;
        quad[3] = bottomLeft;
    }
}

static std::vector<QuadWithMat> GetAllWarpedMarkers(cv::Mat &sourceFrame, std::vector<std::vector<cv::Point>> &allSortedQuads)
{

    if (allSortedQuads.empty())
    {
        return std::vector<QuadWithMat>();
    }

    std::vector<QuadWithMat> warpedMarkers;

    float markerSize = 500;

    std::vector<cv::Point2f> dstCorners =
        {
            cv::Point2f(0.0f, 0.0f),
            cv::Point2f(markerSize, 0.0f),
            cv::Point2f(markerSize, markerSize),
            cv::Point2f(0.0f, markerSize)
        };

    for (auto &quad : allSortedQuads)
    {
        std::vector<cv::Point2f> cornersFloat(quad.begin(), quad.end());

        cv::Mat transformMatrix = cv::getPerspectiveTransform(cornersFloat, dstCorners);

        cv::Mat warpedOutput;
        cv::warpPerspective(sourceFrame, warpedOutput, transformMatrix, cv::Size(markerSize, markerSize), cv::INTER_LINEAR);

        warpedMarkers.push_back({quad, warpedOutput});
    }

    return warpedMarkers;
}

static bool CheckIfBordersAreValid(int pixels[6][6])
{
    // Check if the outer border of the marker is all black (0)
    for (int i = 0; i < 6; ++i)
    {
        if(pixels[0][i] == 1 || pixels[5][i] == 1)
        {
            return false;
        }
    }

    for (int i = 0; i < 6; ++i)
    {
        if(pixels[i][0] == 1 || pixels[i][5] == 1)
        {
            return false;
        }
    }

    return true;
}

static std::vector<QuadWithID> PrintWarpedArucoCodes(std::vector<QuadWithMat> &warpedMarkers)
{
    std::vector<QuadWithID> detectedMarkers;

    for (int i = 0; i < warpedMarkers.size(); ++i)
    {
        int pixels[6][6] = {0};
        cv::Mat gray;
        cv::cvtColor(warpedMarkers[i].matrix, gray, cv::COLOR_BGR2GRAY);

        int imgWidth = warpedMarkers[i].matrix.cols;
        int imgHeight = warpedMarkers[i].matrix.rows;

        int cellwidth = imgWidth / 6;
        int cellheight = imgHeight / 6;

        for (int row = 0; row < 6; ++row)
        {
            for (int col = 0; col < 6; ++col)
            {
                int xLook = col * cellwidth + cellwidth / 2;
                int yLook = row * cellheight + cellheight / 2;  
                
                uchar pixelValue = gray.at<uchar>(yLook, xLook);
                if (pixelValue >= 128)
                {
                    pixels[row][col] = 1;
                }
                else
                {
                    pixels[row][col] = 0;
                }
            }
        }

        
        if (!CheckIfBordersAreValid(pixels))
        {
            continue;
        }

        cv::Mat inner4x4 = cv::Mat::zeros(4, 4, CV_8UC1);
        for (int r = 0; r < 4; ++r) {
           for (int c = 0; c < 4; ++c) {
                inner4x4.at<uchar>(r, c) = static_cast<uchar>(pixels[r + 1][c + 1]);
            }
        }

        cv::aruco::Dictionary dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
        int detectedId = -1;
        int internalRotOffset = 0;

    
        if (!dictionary.identify(inner4x4, detectedId, internalRotOffset, 0.5))
        {
            continue; 
        }

        std::cout << internalRotOffset << std::endl;

        detectedMarkers.push_back({warpedMarkers[i].corners, detectedId, internalRotOffset});

        return detectedMarkers;
    }
    return detectedMarkers;
}

std::vector<QuadWithID> MarkerDetector::detectMarkers(cv::Mat &frame, int minArea, int maxArea, int minQuadArea)
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

    for (auto &contour : contours)
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

    SortCorners(quadrilaterals);

    std::vector<QuadWithMat> warpedMarkers = GetAllWarpedMarkers(frame, quadrilaterals);
    detectedMarkers = PrintWarpedArucoCodes(warpedMarkers);

    cv::Mat markerCorners = frame.clone();
    for (int i = 0; i < detectedMarkers.size(); ++i)
    {
        for (auto &corner : detectedMarkers[i].corners)
        {
            cv::circle(markerCorners, corner, 4, cv::Scalar(0, 255, 255), -1);
        }
    }
    cv::imshow("Edges and Corners", markerCorners);

    return detectedMarkers;
}
