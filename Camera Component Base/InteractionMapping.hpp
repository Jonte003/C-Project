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
        static constexpr double focalLength = 600.0;

        static MarkerPose estimatePose(std::vector<cv::Point> corners);
        static cv::Vec3d toEuler(cv::Vec3d rotationVec);

        static double distanceForQuadArea(double quadArea);

        static double computeZoom(double distance, double nearDistance, double farDistance);
};

#endif
