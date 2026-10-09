#ifndef MARKER_DETECTOR_HPP
#define MARKER_DETECTOR_HPP

#include <opencv2/opencv.hpp>

class MarkerDetector
{
public:
    static void detectMarkers(
        cv::Mat& frame,
        int minArea,
        int maxArea,
        int minQuadArea,
        std::vector<std::vector<cv::Point>>& quadrilaterals
    );
};

#endif
