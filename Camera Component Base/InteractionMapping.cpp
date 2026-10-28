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

cv::Vec3d InteractionMapping::toEuler(cv::Vec3d rotationVec, int rotation)
{
    cv::Matx33d rotationMatrix;
    cv::Rodrigues(rotationVec, rotationMatrix); // Rotation vector to rotation matrix converter from OpenCV

    // The purpose of flip is to adjust the matrix so that the token being
    // upright is considered 0 degrees on the x axis rather than 180 degrees
    cv::Matx33d flip(1,  0,  0,
                     0, -1,  0,
                     0,  0, -1);
    cv::Matx33d adjustedRotationMatrix = flip * rotationMatrix;

    double pitch = std::atan2(adjustedRotationMatrix(2, 1), adjustedRotationMatrix(2, 2));
    double yaw =   std::asin(std::clamp(-adjustedRotationMatrix(2, 0), -1.0, 1.0));
    double roll =  std::atan2(adjustedRotationMatrix(1, 0), adjustedRotationMatrix(0, 0));
    
    double radToDeg = 180.0 / CV_PI;

    roll = roll * radToDeg;
    if(roll < 0) 
    {
        if (rotation == 0) rotation = 4;
        roll = 90 - roll * -1;
        rotation--;
    }
    roll = 360 - (roll + rotation * 90);
    
    return cv::Vec3d(pitch * radToDeg, yaw * radToDeg, roll);
}