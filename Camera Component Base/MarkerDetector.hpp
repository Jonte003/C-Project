#ifndef MARKER_DETECTOR_HPP
#define MARKER_DETECTOR_HPP

#include <opencv2/opencv.hpp>
#include <vector>

class QuadWithID
{
public:
    std::vector<cv::Point> corners;
    int id;
};

class MarkerDetector
{
public:
    static std::vector<QuadWithID> detectMarkers(cv::Mat& frame, int minArea, int maxArea, int minQuadArea);

};

#endif