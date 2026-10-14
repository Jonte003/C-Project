#include "InteractionMapping.hpp"

MarkerPose InteractionMapping::estimatePose(std::vector<cv::Point> corners)
{
    std::vector<cv::Point3f> markerPoints = {
        cv::Point3f(-0.5f, 0.5f, 0.0f),  // Top-left
        cv::Point3f(0.5f, 0.5f, 0.0f),   // Top-right
        cv::Point3f(0.5f, -0.5f, 0.0f),  // Bottom-right
        cv::Point3f(-0.5f, -0.5f, 0.0f)  // Bottom-left
    };

    std::vector<cv::Point2f> imagePoints;
    for (cv::Point corner : corners)
    {
        imagePoints.push_back(cv::Point2f(corner.x, corner.y));
    }

    double focalLength = 600.0;
    cv::Mat cameraMatrix = (cv::Mat_<double>(3, 3) <<
        focalLength, 0, 320,
        0, focalLength, 240,
        0, 0, 1);

    cv::Vec3d rotationVec, translationVec;
    cv::solvePnP(markerPoints, imagePoints, cameraMatrix, 
                 cv::Mat(), rotationVec, translationVec,
                 false, cv::SOLVEPNP_IPPE_SQUARE);

    MarkerPose pose;
    pose.rotation = rotationVec;
    pose.distance = translationVec[2];  // Z component of translation vector represents distance
    return pose;
}
