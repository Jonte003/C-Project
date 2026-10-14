#ifndef INTERACTION_MAPPING_HPP
#define INTERACTION_MAPPING_HPP

#include <vector>
#include <opencv2/opencv.hpp>

struct MarkerPose {
    cv::Vec3d rotation;
    double distance = 0.0;
};

class InteractionMapping {
    public:
        static MarkerPose estimatePose(std::vector<cv::Point> corners);
};

#endif
