#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
#include "CameraInput.hpp"
#include "MarkerDetector.hpp"
#include "InteractionMapping.hpp"
#include "OutputAdapter.hpp"


int main()
{
    int width = 640;
    int height = 480;

    cv::Mat frame;

    int minArea = 500;
    int maxArea = 100000;

    int minQuadArea = 1000;

    if (!CameraInput::init(0, width, height)) 
    {
        return 1;
    }

    while (CameraInput::getFrame(frame)) 
    {
        std::vector<QuadWithID> detectedMarkers = MarkerDetector::detectMarkers(frame, minArea, maxArea, minQuadArea);
        
        for (int i = 0; i < detectedMarkers.size(); ++i) 
        {
            std::cout << "Detected Marker ID: " << detectedMarkers[i].id << std::endl;

            MarkerPose pose = InteractionMapping::estimatePose(detectedMarkers[i].corners);
            cv::Vec3d eulerAngles = InteractionMapping::toEuler(pose.rotation);

            std::cout << std::fixed << std::setprecision(1)
                      << ", Distance:  " << pose.distance
                      << ", X: " << eulerAngles[0]
                      << ", Y: " << eulerAngles[1]
                      << ", Z: " << eulerAngles[2] << std::endl;
        }
        
        cv::imshow("Camera Feed", frame);

        if (cv::waitKey(20) == 27) 
        {
            break;
        }
    }

    CameraInput::release();
    cv::destroyAllWindows();
    return 0;
}
