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
    
    double nearDistance = InteractionMapping::distanceForQuadArea(height * height);
    double farDistance  = InteractionMapping::distanceForQuadArea(minQuadArea);

    if (!CameraInput::init(0, width, height)) 
    {
        return 1;
    }

    OutputAdapter output("127.0.0.1", 9000);
    while (CameraInput::getFrame(frame)) 
    {
        std::vector<QuadWithID> detectedMarkers = MarkerDetector::detectMarkers(frame, minArea, maxArea, minQuadArea);
        
        std::cout << "Number of detected markers: " << detectedMarkers.size() << std::endl;
        
        for (int i = 0; i < detectedMarkers.size(); ++i) 
        {
            std::cout << "Detected Marker ID: " << detectedMarkers[i].id << std::endl;

            MarkerPose pose = InteractionMapping::estimatePose(detectedMarkers[i].corners);
            cv::Vec3d eulerAngles = InteractionMapping::toEuler(pose.rotation);
            double zoom = InteractionMapping::computeZoom(pose.distance, nearDistance, farDistance);

            output.sendMarkerData(detectedMarkers[i].id,
                                  static_cast<float>(zoom),
                                  static_cast<float>(eulerAngles[0]),
                                  static_cast<float>(eulerAngles[1]),
                                  static_cast<float>(eulerAngles[2]));

            std::cout << std::fixed
                      << ", Zoom:  " << std::setprecision(2) << zoom
                      << ", X: "     << std::setprecision(1) << eulerAngles[0]
                      << ", Y: "     <<                         eulerAngles[1]
                      << ", Z: "     <<                         eulerAngles[2] << std::endl;
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
